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
    
    struct terminal* term = create_terminal(framebuffer, tar_open_file((void*)initrd->address, "zap-light24.psf"));
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
	activate_interrupts();
	
    term->puts(term, "Starstone 1.0 Ahlspiess\n");
	
	pit_set_divisor(4096);
	
    while(1) {
        
        term->puts(term, "h");
		sleep(1000);
        // char* text = term->gets(term);
        // if(text) {
            
        //     term->printf(term, "INPUT: %s\n", text);
        //     free(text);
		// 	term->printf(term, "Time since boot: %d\n", get_time_since_boot());
            
        // }
        // else term->puts(term, "ERROR: Failed to read from stdin!\n");
        
    }
    
}
