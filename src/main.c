#include "common.h"
#include "framebuffer.h"
#include "initrd.h"
#include "tar.h"
#include "text_renderer.h"
#include "memory/page_frame_allocator.h"
#include "interrupts/idt.h"
#include "memory/heap.h"
#include "terminal.h"
#include "string.h"

int main(struct framebuffer* framebuffer, struct initrd* initrd, struct memory_map* memory_map) {
    
    init_text_renderer(framebuffer, tar_open_file((void*)initrd->address, "zap-light24.psf"));
    
    if(init_page_frame_allocator(memory_map, framebuffer, initrd)) {
        
        tr_puts("ERROR: Failed to initialize page frame allocator!\n");
        while(1);
        
    }
    
    if(init_heap()) {
        
        tr_puts("ERROR: Failed to initialize heap!\n");
        while(1);
        
    }
    
    create_terminal(framebuffer, tar_open_file((void*)initrd->address, "zap-light24.psf"));
    
    puts("Starstone 1.0 Ahlspiess\n");
    
    if(init_idt()) {
        
        puts("ERROR: Failed to allocate memory for IDT!\n");
        while(1);
        
    }
    
    while(1) {
        
        puts(">>> ");
        char* text = gets();
        if(text) {
            
            printf("INPUT: %s\n", text);
            free(text);
            
        }
        else puts("ERROR: Failed to read from stdin!\n");
        
    }
    
}
