#include "elf.h"
#include "memory/paging.h"
#include "memory/page_frame_allocator.h"
#include "scheduling/multitasking.h"
#include "text_renderer.h"
#include "string.h"
#include "memory/memory.h"

uint8_t start_elf(void* elf, char* name) {
	
	if(!elf) return 1;
	
	if(memcmp(elf, "\x7f""ELF", 4)) return 1;
	
	struct elf64_ehdr* ehdr = elf;
	if(ehdr->e_type != ET_EXEC || ehdr->e_machine != EM_X86_64 || ehdr->e_version != EV_CURRENT) return 1;
	
	struct elf64_phdr* phdr = (struct elf64_phdr*)((uint64_t)elf + ehdr->e_phoff);
	uint64_t phdr_end = (uint64_t)elf + ehdr->e_phoff + ehdr->e_phentsize * ehdr->e_phnum;
	
	uint64_t page_map = create_page_table();
	uint64_t pml4_addr = (uint64_t)&pml4;
	
	asm("mov cr3, %0" : : "r" (page_map));
	
	for(; (uint64_t)phdr < phdr_end; phdr = (struct elf64_phdr*)((uint64_t)phdr + ehdr->e_phentsize)) {
		
		if(phdr->p_type == PT_LOAD) {
			
			uint64_t pages = (phdr->p_memsz + 4095) / 4096;
			for(uint64_t i = 0; i < pages; i++) {
				
				uint64_t page = request_page_frame();
				if(!page || map_page(page, phdr->p_vaddr, 1, 1)) {
					asm("mov cr3, %0" : : "r" (pml4_addr));
					return 1;
				}
				
			}
			memcpy((void*)((uint64_t)elf + phdr->p_offset), (void*)phdr->p_vaddr, phdr->p_filesz);
			
		}
		
	}
	
	asm("mov cr3, %0" : : "r" (pml4_addr));
	return start_task((void*)ehdr->e_entry, name, page_map, (8 * 4) | 3, (8 * 3) | 3);
	
}
