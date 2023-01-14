#pragma once
#include "../common.h"

struct memory_chunk {
    
    struct memory_chunk* prev;
    struct memory_chunk* next;
    uint8_t used;
    
};

uint8_t init_heap();
void* malloc(uint64_t size);
void free(void* mem);
