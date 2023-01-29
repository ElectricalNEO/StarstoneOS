#include "common.h"
#include "framebuffer.h"
#include "initrd.h"
#include "tar.h"
#include "text_renderer.h"
#include "memory/page_frame_allocator.h"
#include "interrupts/idt.h"
#include "memory/heap.h"
#include "terminal.h"
#include "scheduling/pit.h"
#include "scheduling/multitasking.h"
#include "scheduling/task_manager.h"
#include "memory/paging.h"
#include "elf.h"
#include "tss.h"
#include "syscall.h"
#include "string.h"

struct framebuffer* _framebuffer;
struct initrd* _initrd;

extern uint64_t stack;

void init_all(struct framebuffer* framebuffer, struct initrd* initrd, struct memory_map* memory_map) {
	
	_framebuffer = framebuffer;
	_initrd = initrd;
	
	init_text_renderer(tar_open_file((void*)initrd->address, "zap-light24.psf"));
	
    if(init_page_frame_allocator(memory_map, framebuffer, initrd)) {
        
        puts("ERROR: Failed to initialize page frame allocator!\n");
        while(1);
        
    }
    
    if(init_heap()) {
        
        puts("ERROR: Failed to initialize heap!\n");
        while(1);
        
    }
	
	if(init_idt()) {
        
        puts("ERROR: Failed to allocate memory for IDT!\n");
        while(1);
        
    }
	
	init_syscall();
	
	pit_set_divisor(32768);
	flush_tss();
	*(uint16_t*)((uint64_t)&tss + 0x66 + KERNEL_VIRT) = 104;
	
}

char stdout_buf[255];
int16_t rear = -1, front = -1;

void stdout_write(char* buf, uint64_t size) {
	
	if(rear + size >= 255) return;
	if(front == -1) front = 0;
	for(uint64_t i = 0; i < size; i++) {
		rear++;
		stdout_buf[rear] = buf[i];
	}
	
}

void task_terminal() {
	
	lock_task();
	struct terminal* terminal = create_terminal(_framebuffer, tar_open_file((void*)_initrd->address, "zap-light24.psf"));
	if(!terminal) terminate_current_task();
	terminal->puts(terminal, "Terminal\n");
	unlock_task();
	while(1) {
		
		lock_task();
		for(; front > -1 && front <= rear; front++) {
			
			terminal->putc(terminal, stdout_buf[front]);
			
		}
		unlock_task();
		
	}
	
}

void main(struct framebuffer* framebuffer, struct initrd* initrd, struct memory_map* memory_map) {
    
    init_all(framebuffer, initrd, memory_map);
	
	if(start_task(0, "starkrnl", (uint64_t)&pml4, 0x8, 0x10)) {
		
		puts("ERROR: Failed to start kernel task!\n");
		while(1);
		
	}
	
	if(start_task(task_terminal, "Terminal", (uint64_t)&pml4, 0x8, 0x10)) {
		
		puts("ERROR: Failed to start task terminal!\n");
		while(1);
		
	}
	
	if(start_task(task_manager, "Task Manager", (uint64_t)&pml4, 0x8, 0x10)) {
		
		puts("ERROR: Failed to start task manager!\n");
		while(1);
		
	}
	
	if(start_elf(tar_open_file((void*)initrd->address, "app"), "app")) {
		
		puts("ERROR: Failed to start app!\n");
		while(1);
		
	}
	
	activate_interrupts();
	
	while(task_list.next != &task_list);
	lock_task();
	clear();
	puts("All tasks have been terminated.");
	unlock_task();
	
}
