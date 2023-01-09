#include "heap.h"
#include "page_frame_allocator.h"
#include "paging.h"
#include "../text_renderer.h"

struct memory_chunk* heap_start = (struct memory_chunk*)CONSTRUCT_VIRTUAL(511, 511, 0, 0);
uint64_t heap_end = (uint64_t)CONSTRUCT_VIRTUAL(511, 511, 0, 0) + 4096;

uint8_t expand_heap(uint64_t pages) {
    
    if(heap_end + pages == ~0ULL) return 1;
    
    for(uint64_t i = 0; i < pages; i++) {
        
        uint64_t page = request_page_frame();
        if(!page) return 1;
        if(map_page(page, heap_end)) return 1;
        heap_end += 4096;
        
    }
    
    return 0;
    
}

void split_chunk(struct memory_chunk* chunk, uint64_t length) {
    
    struct memory_chunk* new_chunk = (struct memory_chunk*)((uint64_t)chunk + sizeof(struct memory_chunk) + length);
    new_chunk->size = chunk->size - length - sizeof(struct memory_chunk);
    new_chunk->used = 0;
    new_chunk->prev = chunk;
    chunk->size = length;
    
}

void combine_forward(struct memory_chunk* chunk) {
    
    struct memory_chunk* next_chunk = (struct memory_chunk*)((uint64_t)chunk + chunk->size + sizeof(struct memory_chunk));
    chunk->size += sizeof(struct memory_chunk) + next_chunk->size;
    
    struct memory_chunk* next_next_chunk = (struct memory_chunk*)((uint64_t)next_chunk + next_chunk->size + sizeof(struct memory_chunk));
    if((uint64_t)next_next_chunk >= heap_end) return;
    next_next_chunk->prev = chunk;
    
}

uint8_t init_heap() {
    
    uint64_t page = request_page_frame();
    if(!page) return 1;
    if(map_page(page, (uint64_t)heap_start)) return 1;
    
    heap_start->size = 4096 - sizeof(struct memory_chunk);
    heap_start->used = 0;
    heap_start->prev = (struct memory_chunk*)0;
    
    return 0;
    
}

void* malloc(uint64_t size) {
    
    struct memory_chunk* chunk = heap_start;
    while(1) {
        
        if(chunk->size >= size && !chunk->used) {
            
            if(chunk->size > size + sizeof(struct memory_chunk) + 1) split_chunk(chunk, size);
            chunk->used = 1;
            return (void*)((uint64_t)chunk + sizeof(struct memory_chunk));
            
        }
        
        if((uint64_t)chunk + chunk->size + sizeof(struct memory_chunk) >= heap_end) break;
        chunk = (struct memory_chunk*)((uint64_t)chunk + chunk->size + sizeof(struct memory_chunk));
        
    }
    
    uint64_t expand_bytes;
    
    if(!chunk->used) {
        
        expand_bytes = size - chunk->size;
        
    } else {
        
        expand_bytes = size + sizeof(struct memory_chunk);
        
    }
    
    if(expand_heap((expand_bytes + 4095) / 4096)) {
        
        if(!chunk->used) chunk->size = heap_end - (uint64_t)chunk - sizeof(struct memory_chunk);
        else {
            
            struct memory_chunk* new_chunk = (struct memory_chunk*)((uint64_t)chunk + chunk->size + sizeof(struct memory_chunk));
            new_chunk->size = heap_end - (uint64_t)new_chunk - sizeof(struct memory_chunk);
            new_chunk->used = 0;
            new_chunk->prev = chunk;
            
        }
        
        return 0;
        
    }
    
    if(chunk->used) {
        
        struct memory_chunk* new_chunk = (struct memory_chunk*)((uint64_t)chunk + chunk->size + sizeof(struct memory_chunk));
        new_chunk->size = heap_end - (uint64_t)new_chunk - sizeof(struct memory_chunk);
        new_chunk->used = 1;
        new_chunk->prev = chunk;
        if(new_chunk->size > size + sizeof(struct memory_chunk) + 1) split_chunk(new_chunk, size);
        return (void*)((uint64_t)new_chunk + sizeof(struct memory_chunk));
        
    } else {
        
        chunk->size = heap_end - (uint64_t)chunk - sizeof(struct memory_chunk);
        chunk->used = 1;
        if(chunk->size > size + sizeof(struct memory_chunk) + 1) split_chunk(chunk, size);
        return (void*)((uint64_t)chunk + sizeof(struct memory_chunk));
        
    }
    
}

void free(void* mem) {
    
    struct memory_chunk* chunk = (struct memory_chunk*)((uint64_t)mem - sizeof(struct memory_chunk));
    chunk->used = 0;
    if(!chunk->prev->used) combine_forward(chunk->prev);
    
    struct memory_chunk* next_chunk = (struct memory_chunk*)((uint64_t)mem + chunk->size);
    if((uint64_t)next_chunk >= heap_end) return;
    if(!next_chunk->used) combine_forward(chunk);
    
}
