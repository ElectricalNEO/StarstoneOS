#include <memory/memory.hpp>

void memory::memset(void* s, uint64_t n, uint8_t c) {
    
    for(uint64_t i = 0; i < n; i++) {
        
        *(uint8_t*)((uint64_t)s + i) = c;
        
    }
    
}

void memory::memcpy(void* src, void* dst, uint64_t n) {
    
    for(uint64_t i = 0; i < n / 8; i++) {
        
        ((uint64_t*)dst)[i] = ((uint64_t*)src)[i];
        
    }
    
    for(uint8_t i = 0; i < n % 8; i++) {
        
        ((uint64_t*)dst)[n / 8 + i] = ((uint64_t*)src)[n / 8 + i];
        
    }
    
}

uint8_t memory::memcmp(void* a, void* b, uint64_t n) {
	
	for(uint64_t i = 0; i < n; i++) {
		
		if(*(uint8_t*)((uint64_t)a + i) < *(uint8_t*)((uint64_t)b + i)) return -1;
		if(*(uint8_t*)((uint64_t)a + i) > *(uint8_t*)((uint64_t)b + i)) return 1;
		
	}
	
	return 0;
	
}
