#include "memory.h"

char* memory_type_strings[] = {
    "UNKNOWN",
    "AVAILABLE",
    "RESERVED",
    "ACPI RECLAIMABLE",
    "NVS",
    "BADRAM"
};

uint64_t memory_size = 0;

uint64_t get_memory_size(struct memory_map* memory_map) {
    
    if(memory_size) return memory_size;
    
    for(struct memory_map_entry* entry = memory_map->entries; (uint64_t)entry + memory_map->entry_size < (uint64_t)memory_map + memory_map->size; entry = (struct memory_map_entry*)((uint64_t)entry + memory_map->entry_size)) {
        
        memory_size += entry->length;
        
    }
    
    return memory_size;
    
}

void memcpy(void* src, void* dst, uint64_t n) {
    
    for(uint64_t i = 0; i < n / 8; i++) {
        
        ((uint64_t*)dst)[i] = ((uint64_t*)src)[i];
        
    }
    
    for(uint8_t i = 0; i < n % 8; i++) {
        
        ((uint64_t*)dst)[n / 8 + i] = ((uint64_t*)src)[n / 8 + i];
        
    }
    
}

void memset(void* s, uint64_t n, uint8_t c) {
    
    for(uint64_t i = 0; i < n; i++) {
        
        *(uint8_t*)((uint64_t)s + i) = c;
        
    }
    
}
