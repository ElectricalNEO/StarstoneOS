#include "heap.h"
#include "page_frame_allocator.h"
#include "paging.h"
#include "../text_renderer.h"
#include "../terminal.h"

struct memory_chunk* heap_start = (struct memory_chunk*)CONSTRUCT_VIRTUAL(511, 511, 0, 0);
uint64_t heap_end = CONSTRUCT_VIRTUAL(511, 511, 0, 1) - 1;

uint8_t init_heap() {
    
    uint64_t page = request_page_frame();
    if(!page) return 1;
    if(map_page(page, (uint64_t)heap_start, 0)) {
        free_page_frame(page);
        return 1;
    }
    heap_start->next = 0;
    heap_start->prev = 0;
    heap_start->used = 0;
    return 0;
    
}

uint8_t expand_heap(uint64_t pages) {
    
    if(heap_end + pages * 4096 < (uint64_t)heap_end) return 1;
    
    for(uint64_t i = 0; i < pages; i++) {
        
        uint64_t page = request_page_frame();
        if(!page) return 1;
        if(map_page(page, heap_end + 1, 0)) {
            
            free_page_frame(page);
            return 1;
            
        }
        heap_end += 4096;
        
    }
    
    return 0;
    
}

void* malloc(uint64_t size) {
    
    struct memory_chunk* chunk = heap_start;
    while(chunk->next) {
        
        if((uint64_t)chunk->next - (uint64_t)chunk - sizeof(struct memory_chunk) * 2 > size && !chunk->used) {
            
            // chunk found
            
            if((uint64_t)chunk->next - (uint64_t)chunk - sizeof(struct memory_chunk) > size + 0x20) { // split chunk
                
                struct memory_chunk* next = (struct memory_chunk*)((uint64_t)chunk + sizeof(struct memory_chunk) + size);
                next->next = chunk->next;
                chunk->next = next;
                next->prev = chunk;
                next->used = 0;
                
            }
            
            chunk->used = 1;
            return (void*)((uint64_t)chunk + sizeof(struct memory_chunk));
            
        }
        
        chunk = chunk->next;
        
    }
    
    // last chunk reached, expand
    uint64_t chunk_size = heap_end - (uint64_t)chunk - sizeof(struct memory_chunk);
    uint64_t needed_size = size + sizeof(struct memory_chunk);
    if(chunk_size < needed_size) {
        
        uint64_t expand_size = needed_size - chunk_size;
        if(expand_heap((expand_size + 4095) / 4096)) return 0;
        
    }
    
    chunk->used = 1;
    chunk->next = (struct memory_chunk*)((uint64_t)chunk + size + sizeof(struct memory_chunk));
    
    chunk->next->next = 0;
    chunk->next->prev = chunk;
    chunk->next->used = 0;
    
    return (void*)((uint64_t)chunk + sizeof(struct memory_chunk));
    
}

void free(void* mem) {
    
    struct memory_chunk* chunk = (struct memory_chunk*)((uint64_t)mem - sizeof(struct memory_chunk));
    chunk->used = 0;
    if(chunk->next && !chunk->next->used) {
        
        chunk->next = chunk->next->next;
        
    }
    if(chunk->prev && !chunk->prev->used) {
        
        chunk->prev->next = chunk->next;
        
    }
    
}
