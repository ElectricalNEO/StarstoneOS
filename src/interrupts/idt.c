#include "idt.h"
#include "../memory/page_frame_allocator.h"
#include "../memory/paging.h"
#include "../text_renderer.h"
#include "interrupts.h"
#include "pic.h"

struct gate_descriptor* idt = (struct gate_descriptor*)IDT_ADDRESS;

uint8_t init_idt() {
    
    uint64_t idt_phys = request_page_frame();
    if(!idt_phys) return 1;
    if(map_page(idt_phys, IDT_ADDRESS)) return 1;
    
    memset((void*)IDT_ADDRESS, 4096, 0);
    
    for(uint16_t i = 0; i < 256; i++) {
        
        set_interrupt_gate(i, int_ignore);
        
    }
    
    set_interrupt_gate(0x0e, int_0eh);
    set_interrupt_gate(0x20, int_20h);
    set_interrupt_gate(0x21, int_21h);
    
    struct idtr idtr;
    idtr.offset = IDT_ADDRESS;
    idtr.size = 4095;
    
    asm("lidt %0" : : "m" (idtr));
    
    remap_pic();
    
    return 0;
    
}

void set_interrupt_gate(uint8_t i, void* handler) {
    
    idt[i].offset_low = GATE_OFFSET_LOW(handler);
    idt[i].offset_mid = GATE_OFFSET_MID(handler);
    idt[i].offset_high = GATE_OFFSET_HIGH(handler);
    idt[i].flags = GATE_FLAGS(1, 0, INTERRUPT_GATE);
    idt[i].segment = 8;
    
}

void activate_interrupts() {
	
	pic_master_set_mask(0b11111100);
    pic_slave_set_mask(0b11111111);
	asm("sti");
	
}
