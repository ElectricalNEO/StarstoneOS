#include "common.h"
#include "framebuffer.h"
#include "initrd.h"
#include "tar.h"
#include "text_renderer.h"
#include "memory.h"
#include "page_frame_allocator.h"
#include "paging.h"

extern struct gdt_pointer gdt_pointer;

int main(struct framebuffer* framebuffer, struct initrd* initrd, struct memory_map* memory_map) {
    
    init_text_renderer(framebuffer, tar_open_file((void*)initrd->address, "zap-light24.psf"));
    
    puts("Starstone 1.0 Ahlspiess\n");
    
    printf("RAM installed: %d MB\n", get_memory_size(memory_map) / 1024 / 1024);
    
    if(init_page_frame_allocator(memory_map, initrd)) {
        
        printf("ERROR: Failed to initialize page frame allocator!\n");
        while(1);
        
    }
    
    uint64_t a = request_page_frame();
    map_page(a, 0);
    uint8_t* b = 0;
    memset(b, 4096, 0x15);
    
    while(1);
    
}
