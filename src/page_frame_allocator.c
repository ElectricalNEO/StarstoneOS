#include "page_frame_allocator.h"
#include "bitmap.h"
#include "text_renderer.h"
#include "paging.h"

void* page_bitmap = 0;
uint64_t total_pages;
extern void* KERNEL_END;

int init_page_frame_allocator(struct memory_map* memory_map, struct initrd* initrd) {
    
    total_pages = (get_memory_size(memory_map) + 4095) / 4096;
    uint64_t bitmap_size = (total_pages + 7) / 8;
    if(bitmap_size & 0x1fffff) {
        
        bitmap_size &= ~0x1fffffULL;
        bitmap_size += 0x200000;
        
    }
    
    uint64_t page_bitmap_phys = 0;
    
    for(struct memory_map_entry* entry = memory_map->entries; (uint64_t)entry + memory_map->entry_size < (uint64_t)memory_map + memory_map->size; entry = (struct memory_map_entry*)((uint64_t)entry + memory_map->entry_size)) {
        
        uint64_t offset = entry->base;
        if(entry->base == 0x100000) {
            
            offset = (uint64_t)&KERNEL_END - 0xffffffff80000000ULL - 0x100000;
            
        } else if (entry->base == initrd->address) {
            
            offset = initrd->size;
            
        }
        offset &= ~0x1fffffULL;
        offset += 0x200000;
        
        if(offset + entry->length > bitmap_size && entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
            page_bitmap_phys = (void*)(entry->base + offset);
            break;
        }
        
    }
    
    if(!page_bitmap_phys) return 1;
    
    for(uint64_t i = 0; i < bitmap_size / 0x200000; i++) {
        *(uint64_t*)(CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, 511, 0) + (i + 32) * 8) = ((uint64_t)page_bitmap_phys + i * 0x200000) | 0b10000011;
    }
    
    page_bitmap = (void*)CONSTRUCT_VIRTUAL(511, 0, 32, 0);
    
    for(uint64_t i = 0; i < (total_pages + 7) / 8; i++)
        ((uint8_t*)page_bitmap)[i] = 0;
    
    BITMAP_SET(page_bitmap, 0);
    
    for(struct memory_map_entry* entry = memory_map->entries; (uint64_t)entry + memory_map->entry_size < (uint64_t)memory_map + memory_map->size; entry = (struct memory_map_entry*)((uint64_t)entry + memory_map->entry_size)) {
        
        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            
            for(uint64_t i = 0; i < entry->length / 4096; i++) BITMAP_SET(page_bitmap, entry->base / 4096 + i);
            
        }
        
        uint64_t offset = entry->base;
        if(entry->base == 0x100000) {
            
            offset = (uint64_t)&KERNEL_END - 0xffffffff80000000ULL - 0x100000;
            
        } else if (entry->base == initrd->address) {
            
            offset = initrd->size;
            
        } else continue;
        
        offset &= ~0xfffULL;
        offset += 0x1000;
        
        if(offset >= entry->length) continue;
        
        for(uint64_t i = 0; i < (entry->length - offset) / 4096; i++) BITMAP_SET(page_bitmap, entry->base / 4096 + offset / 4096 + i);
        
    }
    
    for(uint64_t i = 0; i < bitmap_size / 4096; i++) {
        
        BITMAP_SET(page_bitmap, i + page_bitmap_phys / 4096);
        
    }
    
    return 0;
    
}

uint64_t request_page_frame() {
    
    for(uint64_t i = 0; i < total_pages; i++) {
        
        if(!BITMAP_GET(page_bitmap, i)) {
            
            BITMAP_SET(page_bitmap, i);
            return i * 4096;
            
        }
        
    }
    
    return 0;
    
}

void free_page_frame(uint64_t addr) {
    
    BITMAP_CLEAR(page_bitmap, addr / 4096);
    
}
