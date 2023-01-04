#include "common.h"
#include "framebuffer.h"
#include "initrd.h"
#include "tar.h"
#include "text_renderer.h"
#include "memory.h"
#include "bitmap.h"

int main(struct framebuffer* framebuffer, struct initrd* initrd, struct memory_map* memory_map) {
    
    init_text_renderer(framebuffer, tar_open_file((void*)(uint64_t)initrd->address, "zap-light24.psf"));
    
    puts("Starstone 1.0 Ahlspiess\n");
    
    uint64_t total_memory = 0;
    
    for(struct memory_map_entry* entry = memory_map->entries; (uint64_t)entry + memory_map->entry_size < (uint64_t)memory_map + memory_map->size; entry = (struct memory_map_entry*)((uint64_t)entry + memory_map->entry_size)) {
        
        printf("0x%x - %s\n", entry->base, memory_type_strings[entry->type < 6 ? entry->type : 0]);
        total_memory += entry->length;
        
    }
    
    printf("RAM installed: %d B\n", total_memory);
    
    uint64_t bitmap = 0;
    
    BITMAP_SET(&bitmap, 1);
    BITMAP_SET(&bitmap, 4);
    BITMAP_SET(&bitmap, 5);
    BITMAP_SET(&bitmap, 10);
    BITMAP_SET(&bitmap, 16);
    BITMAP_SET(&bitmap, 17);
    BITMAP_SET(&bitmap, 20);
    BITMAP_SET(&bitmap, 32);
    BITMAP_SET(&bitmap, 47);
    BITMAP_SET(&bitmap, 48);
    BITMAP_SET(&bitmap, 49);
    BITMAP_SET(&bitmap, 50);
    BITMAP_SET(&bitmap, 51);
    BITMAP_SET(&bitmap, 52);
    BITMAP_CLEAR(&bitmap, 50);
    
    for(uint8_t i = 0; i < 64; i++) {
        
        if(BITMAP_GET(&bitmap, i)) printf("%d\n", i);
        
    }
    
    while(1);
    
}
