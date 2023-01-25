#include "common.h"
#include "framebuffer.h"
#include "initrd.h"
#include "tar.h"
#include "text_renderer.h"
#include "memory/page_frame_allocator.h"
#include "interrupts/idt.h"
#include "memory/heap.h"
#include "terminal.h"
#include "pci.h"
#include "scheduling/pit.h"
#include "scheduling/multitasking.h"

struct terminal* term;

void task_a() {
	
	while(1) term->printf(term, "a");
	
}

void task_b() {
	
	while(1) term->printf(term, "b");
	
}

void task_c() {
	
	while(1) term->printf(term, "c");
	
}

int main(struct framebuffer* framebuffer, struct initrd* initrd, struct memory_map* memory_map) {
    
    init_text_renderer(framebuffer, tar_open_file((void*)initrd->address, "zap-light24.psf"));
	
    if(init_page_frame_allocator(memory_map, framebuffer, initrd)) {
        
        puts("ERROR: Failed to initialize page frame allocator!\n");
        while(1);
        
    }
    
    if(init_heap()) {
        
        puts("ERROR: Failed to initialize heap!\n");
        while(1);
        
    }
    
    term = create_terminal(framebuffer, tar_open_file((void*)initrd->address, "zap-light24.psf"));
	if(!term) {
		
		puts("ERROR: Failed to create a virtual terminal!\n");
		while(1);
		
	}
    
	
	for(uint8_t i = 0; i < 5; i++) {
		
		struct terminal* term = create_terminal(framebuffer, tar_open_file((void*)initrd->address, "zap-light24.psf"));
		term->printf(term, "This is terminal number %d!\n", i +1);
		
	}
    
    if(init_idt()) {
        
        term->puts(term, "ERROR: Failed to allocate memory for IDT!\n");
        while(1);
        
    }	
	pit_set_divisor(4096);
	
    term->puts(term, "Starstone 1.0 Ahlspiess\n");
	
	if(start_task(task_a)) {
		
		term->puts(term, "Failed to start task A!\n");
		while(1);
		
	}
	
	if(start_task(task_b)) {
		
		term->puts(term, "Failed to start task B!\n");
		while(1);
		
	}
	
	if(start_task(task_c)) {
		
		term->puts(term, "Failed to start task C!\n");
		while(1);
		
	}
	
	activate_interrupts();
	
	while(1);
    
}
