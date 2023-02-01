#pragma once
#include <common.hpp>

namespace memory {
	
	class Heap {
		
		struct MemoryChunk {
			
			MemoryChunk* prev;
			MemoryChunk* next;
			uint8_t used;
			
		};
		
		static MemoryChunk* start;
		static uint64_t end;
		
		static uint8_t expand(uint64_t pages);
		Heap();
	public:
		static uint8_t init(uint64_t virtualAddress);
		
		static void* malloc(uint64_t size);
		static void free(void* mem);
		
	};
	
}
