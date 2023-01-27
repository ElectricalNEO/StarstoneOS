#include "program.h"
#include "memory/paging.h"
#include "memory/page_frame_allocator.h"
#include "scheduling/multitasking.h"
#include "text_renderer.h"
#include "string.h"

extern uint64_t pdpt;

uint8_t start_program(void* program, uint64_t size, uint64_t offset, char* name) {
	
	uint64_t page_map = request_page_frame();
	if(!page_map) return 1;
	
	if(map_page(page_map, 0x1000)) return 1;
	memset((void*)0x1000, 0x1000, 0);
	*(uint64_t*)(0x1000 + 511 * 8) = (uint64_t)&pdpt | 0b11;
	*(uint64_t*)(0x1000 + PML4_RECURSIVE * 8) = page_map | 0b11;
	
	asm("mov cr3, %0" : : "r" (page_map));
	
	for(uint64_t i = 0; i < size; i += 0x1000) {
		
		uint64_t page = request_page_frame();
		if(!page) return 1;
		if(map_page(page, offset + i)) return 1;
		
	}
	
	// uint64_t pml4_addr = &pml4;
	// asm("mov cr3, %0" : : "r" (pml4_addr));
	
	memcpy(program, (void*)offset, size);
	return start_task((void*)offset, name, page_map);
	
}
