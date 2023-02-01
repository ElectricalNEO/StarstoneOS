#include <memory/MemoryMap.hpp>

using namespace memory;

static uint64_t memorySize = 0;

uint64_t memory::getMemorySize(MemoryMap* mmap) {
	
	if(memorySize) return memorySize;
	
	for(MemoryMapEntry* entry = mmap->entries; (uint64_t)entry + mmap->entrySize < (uint64_t)mmap + mmap->size; entry = (MemoryMapEntry*)((uint64_t)entry + mmap->entrySize)) {
        
        memorySize += entry->length;
        
    }
    
    return memorySize;
	
}
