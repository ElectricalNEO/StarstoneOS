#include <memory/Heap.hpp>
#include <memory/PageTable.hpp>
#include <memory/PageFrameAllocator.hpp>

using namespace memory;

Heap::MemoryChunk* Heap::start = 0;
uint64_t Heap::end = 0;

uint8_t Heap::init(uint64_t virtualAddress) {
	
	uint64_t page = PageFrameAllocator::requestPageFrame();
    if(!page) return 1;
    if(PageTable::mapPage(page, virtualAddress, PDE_FLAG_RW)) {
        PageFrameAllocator::freePageFrame(page);
        return 1;
    }
	
	start = (MemoryChunk*)virtualAddress;
	end = virtualAddress + 4095;
    start->next = 0;
    start->prev = 0;
    start->used = 0;
    return 0;
	
}

uint8_t Heap::expand(uint64_t pages) {
	
	if(end + pages * 4096 < end) return 1;
    
    for(uint64_t i = 0; i < pages; i++) {
        
        uint64_t page = PageFrameAllocator::requestPageFrame();
        if(!page) return 1;
        if(PageTable::mapPage(page, end + 1, PDE_FLAG_RW)) {
            
            PageFrameAllocator::freePageFrame(page);
            return 1;
            
        }
        end += 4096;
        
    }
    
    return 0;
	
}

void* Heap::malloc(uint64_t size) {
	
	MemoryChunk* chunk = start;
    while(chunk->next) {
        
        if((uint64_t)chunk->next - (uint64_t)chunk - sizeof(MemoryChunk) * 2 > size && !chunk->used) {
            
            // chunk found
            
            if((uint64_t)chunk->next - (uint64_t)chunk - sizeof(MemoryChunk) > size + 0x20) { // split chunk
                
                MemoryChunk* next = (MemoryChunk*)((uint64_t)chunk + sizeof(MemoryChunk) + size);
                next->next = chunk->next;
                chunk->next = next;
                next->prev = chunk;
                next->used = 0;
                
            }
            
            chunk->used = 1;
            return (void*)((uint64_t)chunk + sizeof(MemoryChunk));
            
        }
        
        chunk = chunk->next;
        
    }
    
    // last chunk reached, expand
    uint64_t chunk_size = end - (uint64_t)chunk - sizeof(MemoryChunk);
    uint64_t needed_size = size + sizeof(MemoryChunk);
    if(chunk_size < needed_size) {
        
        uint64_t expand_size = needed_size - chunk_size;
        if(expand((expand_size + 4095) / 4096)) return 0;
        
    }
    
    chunk->used = 1;
    chunk->next = (MemoryChunk*)((uint64_t)chunk + size + sizeof(MemoryChunk));
    
    chunk->next->next = 0;
    chunk->next->prev = chunk;
    chunk->next->used = 0;
    
    return (void*)((uint64_t)chunk + sizeof(MemoryChunk));
	
}

void Heap::free(void* mem) {
	
	MemoryChunk* chunk = (MemoryChunk*)((uint64_t)mem - sizeof(MemoryChunk));
    chunk->used = 0;
    if(chunk->next && !chunk->next->used) {
        
        chunk->next = chunk->next->next;
        
    }
    if(chunk->prev && !chunk->prev->used) {
        
        chunk->prev->next = chunk->next;
        
    }
	
}
