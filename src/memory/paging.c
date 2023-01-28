#include "paging.h"
#include "page_frame_allocator.h"
#include "../string.h"

uint64_t create_page_table() {
	
	uint64_t page = request_page_frame();
	if(!page) return 0;
	map_page(page, 0xfffffffffffff000, 1, 0);
	memset((void*)0xfffffffffffff000, 4096, 0);
	*(uint64_t*)(0xfffffffffffff000 + 511 * 8) = (uint64_t)&pdpt | PDE_FLAG_PRESENT | PDE_FLAG_RW;
	*(uint64_t*)(0xfffffffffffff000 + PML4_RECURSIVE * 8) = page | PDE_FLAG_PRESENT | PDE_FLAG_RW;
	unmap_page(0xfffffffffffff000);
	return page;
	
}

uint8_t map_page(uint64_t physical_address, uint64_t virtual_address, uint8_t rw, uint8_t user) {
    
    virtual_address >>= 12;
    uint16_t pt_i = virtual_address & 0b111111111;
    virtual_address >>= 9;
    uint16_t pd_i = virtual_address & 0b111111111;
    virtual_address >>= 9;
    uint16_t pdpt_i = virtual_address & 0b111111111;
    virtual_address >>= 9;
    uint16_t pml4_i = virtual_address & 0b111111111;
    
	user = (user & 1) * PDE_FLAG_US;
	rw = (rw & 1) * PDE_FLAG_RW;
	
    uint64_t* pml4 = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE);
    if(!PDE_GET_FLAG(pml4[pml4_i], PDE_FLAG_PRESENT)) {
        
        uint64_t page = request_page_frame();
        if(!page) return 1;
        pml4[pml4_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | PDE_FLAG_US | page;
		uint64_t pdpt = CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE, pml4_i);
		asm("invlpg [(%0)]" : : "m" (pdpt));
        memset((void*)pdpt, 4096, 0);
        
    }
    pml4[pml4_i] |= PDE_FLAG_RW | PDE_FLAG_US;
    uint64_t* pdpt = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE, pml4_i);
    if(!PDE_GET_FLAG(pdpt[pdpt_i], PDE_FLAG_PRESENT)) {
        
        uint64_t page = request_page_frame();
        if(!page) return 1;
        pdpt[pdpt_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | PDE_FLAG_US | page;
		uint64_t pd = CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, pml4_i, pdpt_i);
		asm("invlpg [(%0)]" : : "m" (pd));
        memset((void*)pd, 4096, 0);
        
    }
	
	
    pdpt[pdpt_i] |= PDE_FLAG_RW | PDE_FLAG_US;
    
    uint64_t* pd = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, pml4_i, pdpt_i);
    if(!PDE_GET_FLAG(pd[pd_i], PDE_FLAG_PRESENT)) {
        
        uint64_t page = request_page_frame();
        if(!page) return 1;
        pd[pd_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | PDE_FLAG_US | page;
		uint64_t pt = CONSTRUCT_VIRTUAL(PML4_RECURSIVE, pml4_i, pdpt_i, pd_i);
		asm("invlpg [(%0)]" : : "m" (pt));
        memset((void*)pt, 4096, 0);
        
    }
    pd[pd_i] |= PDE_FLAG_RW | PDE_FLAG_US;
    
    uint64_t* pt = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, pml4_i, pdpt_i, pd_i);
    pt[pt_i] = PDE_FLAG_PRESENT | rw | user | physical_address;
	asm("invlpg [(%0)]" : : "m" (virtual_address));
	
    return 0;
    
}

void unmap_page(uint64_t virtual_address) {
	
	virtual_address >>= 12;
    uint16_t pt_i = virtual_address & 0b111111111;
    virtual_address >>= 9;
    uint16_t pd_i = virtual_address & 0b111111111;
    virtual_address >>= 9;
    uint16_t pdpt_i = virtual_address & 0b111111111;
    virtual_address >>= 9;
    uint16_t pml4_i = virtual_address & 0b111111111;
    
    uint64_t* pml4 = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE);
    if(!PDE_GET_FLAG(pml4[pml4_i], PDE_FLAG_PRESENT)) return;
	
    uint64_t* pdpt = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE, pml4_i);
    if(!PDE_GET_FLAG(pdpt[pdpt_i], PDE_FLAG_PRESENT)) return;
    
    uint64_t* pd = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, pml4_i, pdpt_i);
    if(!PDE_GET_FLAG(pd[pd_i], PDE_FLAG_PRESENT)) return;
    
    uint64_t* pt = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, pml4_i, pdpt_i, pd_i);
    pt[pt_i] = 0;
	
	asm("invlpg [(%0)]" : : "m" (virtual_address));
	
}
