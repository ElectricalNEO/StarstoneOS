#include <memory/bitmap.hpp>
#include <memory/memory.hpp>
#include <memory/PageTable.hpp>
#include <memory/PageFrameAllocator.hpp>

using namespace memory;

extern void* KERNEL_END;

uint64_t PageFrameAllocator::totalPages;
uint64_t PageFrameAllocator::freeMem;
uint64_t PageFrameAllocator::usedMem;

uint8_t PageFrameAllocator::init(Framebuffer* framebuffer, Initrd* initrd, MemoryMap* mmap) {
	
	totalPages = (getMemorySize(mmap) + 4095) / 4096;
    uint64_t bitmapSize = (totalPages + 7) / 8;
    uint64_t bitmapPages = (bitmapSize + 4095) / 4096;
    uint64_t bitmapPageFrames[bitmapPages];
    uint64_t curPage = 0;
    freeMem = getMemorySize(0);
	
    for(MemoryMapEntry* entry = mmap->entries; (uint64_t)entry + mmap->entrySize < (uint64_t)mmap + mmap->size; entry = (MemoryMapEntry*)((uint64_t)entry + mmap->entrySize)) {
        
        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE) continue;
        for(uint64_t page = (entry->base + 0x1fffff) / 0x200000 * 0x200000; page + 0x200000 < entry->base + entry->length; page+=0x200000) {
            
            if(page >= 0x100000 && page < (uint64_t)&KERNEL_END - KERNEL_VIRT) continue;
            if(page >= (uint64_t)initrd->address - INITRD_VIRT && page < (uint64_t)initrd->address - INITRD_VIRT + initrd->size) continue;
            bitmapPageFrames[curPage] = page;
            curPage++;
            if(curPage == bitmapPages) goto end;
            
        }
        
    }
	
    end:
	
    if(curPage < bitmapPages) return 1;
    
    for(uint64_t i = 0; i < bitmapPages; i++) {
        *(uint64_t*)(0xffffffffffc00000 + (i + 32) * 8) = bitmapPageFrames[i] | 0b10000011;
    }
    
	
    memory::memset((void*)PAGE_BITMAP, bitmapSize, 0);
    BITMAP_SET(PAGE_BITMAP, 0);
    
    for(MemoryMapEntry* entry = mmap->entries; (uint64_t)entry + mmap->entrySize < (uint64_t)mmap + mmap->size; entry = (MemoryMapEntry*)((uint64_t)entry + mmap->entrySize)) {
        
        if(entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            
            for(uint64_t i = 0; i < entry->length / 4096; i++) BITMAP_SET(PAGE_BITMAP, entry->base / 4096 + i);
            
        }
        
    }
    
    for(uint64_t i = 0; i < ((uint64_t)&KERNEL_END - KERNEL_VIRT - 0x100000 + 4095) / 4096; i++) {
        
		usedMem += 4096;
		freeMem -= 4096;
        BITMAP_SET(PAGE_BITMAP, 0x100000 + i);
        
    }
    
    for(uint64_t i = ((uint64_t)initrd->address - INITRD_VIRT) / 4096; i < ((uint64_t)initrd->address - INITRD_VIRT) / 4096 + (initrd->size + 4095) / 4096; i++) {
        
		usedMem += 4096;
		freeMem -= 4096;
        BITMAP_SET(PAGE_BITMAP, i);
        
    }
    
    for(uint64_t i = (framebuffer->address - FRAMEBUFFER_VIRT) / 4096; i < (framebuffer->address - FRAMEBUFFER_VIRT) / 4096 + ((framebuffer->pitch * framebuffer->height * framebuffer->bpp / 8) + 4095) / 4096; i++) {
        
		usedMem += 4096;
		freeMem -= 4096;
        BITMAP_SET(PAGE_BITMAP, i);
        
    }
    
    return 0;
	
}

uint64_t PageFrameAllocator::requestPageFrame() {
	
	if(!freeMem) return 0;
	
	for(uint64_t i = 0; i < totalPages; i++) {
		
		if(!BITMAP_GET(PAGE_BITMAP, i)) {
			
			usedMem += 4096;
			freeMem -= 4096;
			BITMAP_SET(PAGE_BITMAP, i);
			return i * 4096;
			
		}
		
	}
	
	return 0;
	
}

void PageFrameAllocator::freePageFrame(uint64_t page) {
	
	if(!BITMAP_GET(PAGE_BITMAP, page / 4096)) return;
	usedMem -= 4096;
	freeMem += 4096;
	BITMAP_CLEAR(PAGE_BITMAP, page / 4096);
	
}

uint64_t PageFrameAllocator::getFreeMemory() {
	
	return freeMem;
	
}

uint64_t PageFrameAllocator::getUsedMemory() {
	
	return usedMem;
	
}
