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
#include "program.h"

struct framebuffer* _framebuffer;
struct initrd* _initrd;

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
	
	pit_set_divisor(32768);
	
}

struct terminal* terminal;

void task_terminal() {
	
	terminal = create_terminal(_framebuffer, tar_open_file((void*)_initrd->address, "zap-light24.psf"));
	if(!terminal) return;
	terminal->puts(terminal, "Terminal\n");
	while(1);
	
}

void a() {while(1);}

void main(struct framebuffer* framebuffer, struct initrd* initrd, struct memory_map* memory_map) {
    
    init_all(framebuffer, initrd, memory_map);
	
	if(start_task(task_manager, "Task Manager", (uint64_t)&pml4)) {
		
		puts("ERROR: Failed to start task manager!\n");
		while(1);
		
	}
	
	if(start_task(task_terminal, "Terminal", (uint64_t)&pml4)) {
		
		puts("ERROR: Failed to start terminal!\n");
		while(1);
		
	}
	
	if(start_program(tar_open_file((void*)initrd->address, "app.bin"), tar_get_file_size((void*)initrd->address, "app.bin"), 0x1000, "app.bin")) {
		
		puts("ERROR: Failed to start app.bin!\n");
		while(1);
		
	}
	
	activate_interrupts();
    
}
