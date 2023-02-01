#pragma once
#include <common.hpp>

namespace memory {
	
	void memset(void* s, uint64_t n, uint8_t c);
	void memcpy(void* src, void* dst, uint64_t n);
	uint8_t memcmp(void* a, void* b, uint64_t n);
	
}
