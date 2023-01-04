#include "memory.h"

char* memory_type_strings[] = {
    "UNKNOWN",
    "AVAILABLE",
    "RESERVED",
    "ACPI RECLAIMABLE",
    "NVS",
    "BADRAM"
};

void memcpy(void* src, void* dst, uint64_t n) {
    
    for(uint64_t i = 0; i < n; i++) {
        
        *(uint8_t*)((uint64_t)dst + i) = *(uint8_t*)((uint64_t)src + i);
        
    }
    
}
