#pragma once
#include <common.hpp>
#include <Initrd.hpp>
#include <Framebuffer.hpp>
#include <memory/MemoryMap.hpp>

#define PAGE_BITMAP 0xffffff0004000000

namespace memory {
	
	class PageFrameAllocator {
		
		static uint64_t totalPages;
		static uint64_t freeMem;
		static uint64_t usedMem;
		
		PageFrameAllocator();
	public:
		static uint8_t init(Framebuffer* framebuffer, Initrd* initrd, MemoryMap* mmap);
		static uint64_t requestPageFrame();
		static void freePageFrame(uint64_t page);
		static uint64_t getFreeMemory();
		static uint64_t getUsedMemory();
		
	};
	
}
