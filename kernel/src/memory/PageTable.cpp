#include <memory/memory.hpp>
#include <memory/PageTable.hpp>
#include <memory/PageFrameAllocator.hpp>

using namespace memory;

PageTable memory::kernelPageTable;

PageTable::PageTable() {
	
	asm("mov %0, cr3" : "=r" (pml4));
	
}

PageTable::PageTable(uint64_t pml4) {
	
	this->pml4 = pml4;
	
	mapPage(pml4, 0, PDE_FLAG_RW);
	memcpy((uint64_t*)0xfffffffffffff000, 0, 4096);
	unmapPage(0);
	
}

uint8_t PageTable::mapPage(uint64_t physicalAddress, uint64_t virtualAddress, uint64_t flags) {
	
	virtualAddress >>= 12;
    uint16_t pt_i = virtualAddress & 0b111111111;
    virtualAddress >>= 9;
    uint16_t pd_i = virtualAddress & 0b111111111;
    virtualAddress >>= 9;
    uint16_t pdpt_i = virtualAddress & 0b111111111;
    virtualAddress >>= 9;
    uint16_t pml4_i = virtualAddress & 0b111111111;
    
    uint64_t* pml4 = (uint64_t*)0xfffffffffffff000;
	uint64_t* pdpt = (uint64_t*)(0xffffffffffe00000 | (pml4_i << 12));
	uint64_t* pd = (uint64_t*)(0xffffffffc0000000 | (pml4_i << 21) | (pdpt_i << 12));
	uint64_t* pt = (uint64_t*)(0xffffff8000000000 | (pml4_i << 30) | (pdpt_i << 21) | (pd_i << 12));
	
    if(!PDE_GET_FLAG(pml4[pml4_i], PDE_FLAG_PRESENT)) {
        
        uint64_t page = PageFrameAllocator::requestPageFrame();
        if(!page) return 1;
        pml4[pml4_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | page;
		asm("invlpg [(%0)]" : : "m" (pdpt));
        memset(pdpt, 4096, 0);
        
    }
    pml4[pml4_i] |= PDE_FLAG_RW | PDE_FLAG_US;
	
    if(!PDE_GET_FLAG(pdpt[pdpt_i], PDE_FLAG_PRESENT)) {
        
        uint64_t page = PageFrameAllocator::requestPageFrame();
        if(!page) return 1;
        pdpt[pdpt_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | page;
		asm("invlpg [(%0)]" : : "m" (pd));
        memset(pd, 4096, 0);
        
    }
    pdpt[pdpt_i] |= PDE_FLAG_RW | PDE_FLAG_US;
    
    if(!PDE_GET_FLAG(pd[pd_i], PDE_FLAG_PRESENT)) {
        
        uint64_t page = PageFrameAllocator::requestPageFrame();
        if(!page) return 1;
        pd[pd_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | page;
		asm("invlpg [(%0)]" : : "m" (pt));
        memset(pt, 4096, 0);
        
    }
    pd[pd_i] |= PDE_FLAG_RW | PDE_FLAG_US;
    
    pt[pt_i] = PDE_FLAG_PRESENT | physicalAddress | flags;
	asm("invlpg [(%0)]" : : "m" (virtualAddress));
	
    return 0;
	
}

void PageTable::unmapPage(uint64_t virtualAddress) {
	
	virtualAddress >>= 12;
	uint16_t pt_i = virtualAddress & 0b111111111;
	virtualAddress >>= 9;
	uint16_t pd_i = virtualAddress & 0b111111111;
	virtualAddress >>= 9;
	uint16_t pdpt_i = virtualAddress & 0b111111111;
	virtualAddress >>= 9;
	uint16_t pml4_i = virtualAddress & 0b111111111;
	
	uint64_t* pml4 = (uint64_t*)0xfffffffffffff000;
	uint64_t* pdpt = (uint64_t*)(0xffffffffffe00000 | (pml4_i << 12));
	uint64_t* pd = (uint64_t*)(0xffffffffc0000000 | (pml4_i << 21) | (pdpt_i << 12));
	uint64_t* pt = (uint64_t*)(0xffffff8000000000 | (pml4_i << 30) | (pdpt_i << 21) | (pd_i << 12));
	
	if(!PDE_GET_FLAG(pml4[pml4_i], PDE_FLAG_PRESENT)) return;
	if(!PDE_GET_FLAG(pdpt[pdpt_i], PDE_FLAG_PRESENT)) return;
	if(!PDE_GET_FLAG(pd[pd_i], PDE_FLAG_PRESENT)) return;
	pt[pt_i] = 0;
	
	asm("invlpg [(%0)]" : : "m" (virtualAddress));
	
}

void PageTable::activate() {
	
	asm("mov cr3, %0" : : "r"(this->pml4));
	
}
