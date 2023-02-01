#pragma once
#include <common.hpp>

namespace multitasking {
	
	class ProgramLoader {
		
	public:
		static uint8_t loadRaw(void* program, uint64_t size, uint64_t offset);
		static uint8_t loadELF(void* program);
		
	};
	
}
