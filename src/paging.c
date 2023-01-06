#include "paging.h"
#include "text_renderer.h"
#include "page_frame_allocator.h"

void map_page(uint64_t physical_address, uint64_t virtual_address) {
    
    virtual_address >>= 12;
    uint16_t pt_i = virtual_address & 0b111111111;
    virtual_address >>= 9;
    uint16_t pd_i = virtual_address & 0b111111111;
    virtual_address >>= 9;
    uint16_t pdpt_i = virtual_address & 0b111111111;
    virtual_address >>= 9;
    uint16_t pml4_i = virtual_address & 0b111111111;
    
    uint64_t* pml4 = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE);
    if(!PDE_GET_FLAG(pml4[pml4_i], PDE_FLAG_PRESENT)) {
        
        pml4[pml4_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | request_page_frame();
        
    }
    
    uint64_t* pdpt = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, PML4_RECURSIVE, pml4_i);
    if(!PDE_GET_FLAG(pdpt[pdpt_i], PDE_FLAG_PRESENT)) {
        
        pdpt[pdpt_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | request_page_frame();
        
    }
    
    uint64_t* pd = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, PML4_RECURSIVE, pml4_i, pdpt_i);
    if(!PDE_GET_FLAG(pd[pd_i], PDE_FLAG_PRESENT)) {
        
        pd[pd_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | request_page_frame();
        
    }
    
    uint64_t* pt = (uint64_t*)CONSTRUCT_VIRTUAL(PML4_RECURSIVE, pml4_i, pdpt_i, pd_i);
    pt[pt_i] = PDE_FLAG_PRESENT | PDE_FLAG_RW | physical_address;
    
}
