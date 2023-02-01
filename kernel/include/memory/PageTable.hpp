#pragma once
#include <common.hpp>

#define KERNEL_VIRT 0xffffff0000000000
#define FRAMEBUFFER_VIRT 0xffffff0040000000
#define INITRD_VIRT 0xffffff0080000000

#define PDE_GET_FLAG(pde, flag) (pde & (1 << flag))

#define PDE_FLAG_PRESENT (1 << 0)
#define PDE_FLAG_RW (1 << 1)
#define PDE_FLAG_US (1 << 2)

extern "C" uint64_t pml4;

namespace memory {
	
	class PageTable {
		
	public:
		uint64_t pml4;
		PageTable();
		PageTable(uint64_t pml4);
		static uint8_t mapPage(uint64_t physicalAddress, uint64_t virtualAddress, uint64_t flags);
		static void unmapPage(uint64_t virtualAddress);
		void activate();
		
	};
	
	extern PageTable kernelPageTable;
	
}
