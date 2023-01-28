#include "page_frame_allocator.h"
#include "bitmap.h"
#include "paging.h"

void* page_bitmap = 0;
uint64_t total_pages;
extern void* KERNEL_END;

uint64_t free_mem = 0;
uint64_t used_mem = 0;

int init_page_frame_allocator(struct memory_map* memory_map, struct framebuffer* framebuffer, struct initrd* initrd) {
    
    total_pages = (get_memory_size(memory_map) + 4095) / 4096;
    uint64_t bitmap_size = (total_pages + 7) / 8;
    uint64_t bitmap_pages = (bitmap_size + 4095) / 4096;
    uint64_t bitmap_page_frames[bitmap_pages];
    uint64_t cur_page = 0;
    free_mem = get_memory_size(0);
	
    for(struct memory_map_entry* entry = memory_map->entries; (uint64_t)entry + memory_map->entry_size < (uint64_t)memory_map + memory_map->size; entry = (struct memory_map_entry*)((uint64_t)entry + memory_map->entry_size)) {
        
        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE) continue;
        for(uint64_t page = (entry->base + 0x1fffff) / 0x200000 * 0x200000; page + 0x200000 < entry->base + entry->length; page+=0x200000) {
            
            if(page >= 0x100000 && page < (uint64_t)&KERNEL_END - KERNEL_VIRT) continue;
            if(page >= initrd->address - INITRD_VIRT && page < initrd->address - INITRD_VIRT + initrd->size) continue;
            bitmap_page_frames[cur_page] = page;
            cur_page++;
            if(cur_page == bitmap_pages) goto end;
            
        }
        
    }
	
    end:
	
    if(cur_page < bitmap_pages) return 1;
    
    for(uint64_t i = 0; i < bitmap_pages; i++) {
        *(uint64_t*)(CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, 511, 0) + (i + 32) * 8) = bitmap_page_frames[i] | 0b10000011;
    }
    
    page_bitmap = (void*)CONSTRUCT_VIRTUAL(511, 0, 32, 0);
    
    memset(page_bitmap, bitmap_size, 0);
    BITMAP_SET(page_bitmap, 0);
    
    for(struct memory_map_entry* entry = memory_map->entries; (uint64_t)entry + memory_map->entry_size < (uint64_t)memory_map + memory_map->size; entry = (struct memory_map_entry*)((uint64_t)entry + memory_map->entry_size)) {
        
        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            
            for(uint64_t i = 0; i < entry->length / 4096; i++) BITMAP_SET(page_bitmap, entry->base / 4096 + i);
            
        }
        
    }
    
    for(uint64_t i = 0; i < ((uint64_t)&KERNEL_END - KERNEL_VIRT - 0x100000 + 4095) / 4096; i++) {
        
		used_mem += 4096;
		free_mem -= 4096;
        BITMAP_SET(page_bitmap, 0x100000 + i);
        
    }
    
    for(uint64_t i = (initrd->address - INITRD_VIRT) / 4096; i < (initrd->address - INITRD_VIRT) / 4096 + (initrd->size + 4095) / 4096; i++) {
        
		used_mem += 4096;
		free_mem -= 4096;
        BITMAP_SET(page_bitmap, i);
        
    }
    
    for(uint64_t i = (framebuffer->address - FRAMEBUFFER_VIRT) / 4096; i < (framebuffer->address - FRAMEBUFFER_VIRT) / 4096 + ((framebuffer->pitch * framebuffer->height * framebuffer->bpp / 8) + 4095) / 4096; i++) {
        
		used_mem += 4096;
		free_mem -= 4096;
        BITMAP_SET(page_bitmap, i);
        
    }
    
    return 0;
    
}

uint64_t request_page_frame() {
    
	if(!free_mem) return 0;
	
    for(uint64_t i = 0; i < total_pages; i++) {
        
        if(!BITMAP_GET(page_bitmap, i)) {
            
			used_mem += 4096;
			free_mem -= 4096;
            BITMAP_SET(page_bitmap, i);
            return i * 4096;
            
        }
        
    }
    
    return 0;
    
}

void free_page_frame(uint64_t addr) {
    
	used_mem -= 4096;
	free_mem += 4096;
    BITMAP_CLEAR(page_bitmap, addr / 4096);
    
}

uint64_t get_free_memory() {
	
	return free_mem;
	
}

uint64_t get_used_memory() {
	
	return used_mem;
	
}
