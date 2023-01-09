#include "common.h"
#include "framebuffer.h"
#include "initrd.h"
#include "tar.h"
#include "text_renderer.h"
#include "memory/memory.h"
#include "memory/page_frame_allocator.h"
#include "interrupts/idt.h"
#include "input/keyboard.h"
#include "memory/heap.h"

int main(struct framebuffer* framebuffer, struct initrd* initrd, struct memory_map* memory_map) {
    
    init_text_renderer(framebuffer, tar_open_file((void*)initrd->address, "zap-light24.psf"));
    
    puts("Starstone 1.0 Ahlspiess\n");
    
    printf("RAM installed: %d MB\n", get_memory_size(memory_map) / 1024 / 1024);
    
    if(init_page_frame_allocator(memory_map, initrd)) {
        
        printf("ERROR: Failed to initialize page frame allocator!\n");
        while(1);
        
    }
    
    if(init_idt()) {
        
        printf("ERROR: Failed to allocate memory for IDT!\n");
        while(1);
        
    }
    
    if(init_heap()) {
        
        printf("ERROR: Failed to initialize heap!\n");
        while(1);
        
    }
    
    void* mem = malloc(4096);
    printf("0x%x\n", mem);
    void* mem1 = malloc(4096);
    printf("0x%x\n", mem1);
    free(mem);
    free(mem1);
    mem = malloc(8192);
    printf("0x%x\n", mem);
    mem = malloc(4000);
    printf("0x%x\n", mem);
    
    // printf(">>> ");
    // while(1) {
        
    //     printf("%c", getc());
        
    // }
    
    while(1);
    
}
