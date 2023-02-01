#pragma once
#include <common.hpp>

#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

namespace memory {
	
	struct MemoryMapEntry {
		
		uint64_t base;
		uint64_t length;
		uint32_t type;
		uint32_t reserved;
		
	}__attribute__((packed));

	struct MemoryMap {
		
		uint32_t size;
		uint32_t entrySize;
		uint32_t entryVersion;
		MemoryMapEntry entries[0];
		
	}__attribute__((packed));

	uint64_t getMemorySize(MemoryMap* mmap);
	
}
