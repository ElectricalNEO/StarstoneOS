#include <elf.hpp>
#include <TextRenderer.hpp>
#include <memory/memory.hpp>
#include <memory/PageTable.hpp>
#include <multitasking/Scheduler.hpp>
#include <memory/PageFrameAllocator.hpp>
#include <multitasking/ProgramLoader.hpp>

using namespace multitasking;

uint8_t ProgramLoader::loadRaw(void* program, uint64_t size, uint64_t offset) {
	
	uint64_t pml4 = memory::PageFrameAllocator::requestPageFrame();
	if(!pml4) return 1;
	
	uint64_t stack = memory::PageFrameAllocator::requestPageFrame();
	if(!stack) return 1;
	
	memory::PageTable pageTable(pml4);
	pageTable.activate();
	
	if(memory::PageTable::mapPage(stack, 0xc0000000, PDE_FLAG_RW | PDE_FLAG_US)) return 1;
	
	for(uint64_t i = 0; i < size; i += 4096) {
		
		uint64_t page = memory::PageFrameAllocator::requestPageFrame();
		if(!page) return 1;
		if(memory::PageTable::mapPage(page, offset + i, PDE_FLAG_RW | PDE_FLAG_US)) return 1;
		memory::memcpy((void*)((uint64_t)program + i), (void*)(offset + i), 4096);
		
	}
	
	memory::kernelPageTable.activate();
	return Scheduler::startTask(offset, 0xc0000fff, 0x20 | 3, 0x18 | 3, &pageTable);
	
}

uint8_t ProgramLoader::loadELF(void* program) {
	
	if(memory::memcmp(program, (void*)"\x7f""ELF", 4)) return 1;
	
	ELF64_Ehdr* ehdr = (ELF64_Ehdr*)program;
	if(ehdr->e_type != ET_EXEC || ehdr->e_machine != EM_X86_64 || ehdr->e_version != EV_CURRENT) return 1;
	
	ELF64_Phdr* phdr = (ELF64_Phdr*)((uint64_t)program + ehdr->e_phoff);
	uint64_t phdrEnd = (uint64_t)program + ehdr->e_phoff + ehdr->e_phentsize * ehdr->e_phnum;
	
	uint64_t pml4 = memory::PageFrameAllocator::requestPageFrame();
	if(!pml4) return 1;
	
	uint64_t stack = memory::PageFrameAllocator::requestPageFrame();
	if(!stack) return 1;
	
	memory::PageTable pageTable(pml4);
	pageTable.activate();
	
	if(memory::PageTable::mapPage(stack, 0xc0000000, PDE_FLAG_RW | PDE_FLAG_US)) return 1;
	
	for(; (uint64_t)phdr < phdrEnd; phdr = (ELF64_Phdr*)((uint64_t)phdr + ehdr->e_phentsize)) {
		
		if(phdr->p_type == PT_LOAD) {
			
			uint64_t pages = (phdr->p_memsz + 4095) / 4096;
			for(uint64_t i = 0; i < pages; i++) {
				
				uint64_t page = memory::PageFrameAllocator::requestPageFrame();
				if(!page || memory::PageTable::mapPage(page, phdr->p_vaddr, PDE_FLAG_RW | PDE_FLAG_US)) {
					memory::kernelPageTable.activate();
					return 1;
				}
				
			}
			memory::memcpy((void*)((uint64_t)program + phdr->p_offset), (void*)phdr->p_vaddr, phdr->p_filesz);
			
		}
		
	}
	
	memory::kernelPageTable.activate();
	return Scheduler::startTask(0x401000, 0xc0000fff, 8, 16, &pageTable);
	
}
