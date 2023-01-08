#include "idt.h"
#include "page_frame_allocator.h"
#include "paging.h"
#include "text_renderer.h"
#include "interrupts.h"
#include "pic.h"

struct gate_descriptor* idt = 0;

void init_idt() {
    
    uint64_t idt_phys = request_page_frame();
    
    map_page(idt_phys, 0);
    memset(0, 4096, 0);
    
    for(uint8_t i = 0; i < 255; i++) {
        
        set_interrupt_gate(i, int_ignore);
        
    }
    
    set_interrupt_gate(0x0e, int_0eh);
    set_interrupt_gate(0x21, int_21h);
    
    struct idtr idtr;
    idtr.offset = 0;
    idtr.size = 4095;
    
    asm("lidt %0" : : "m" (idtr));
    
    remap_pic();
    pic_master_set_mask(0b11111101);
    pic_slave_set_mask(0b11111111);
    
    asm("sti");
    
}

void set_interrupt_gate(uint8_t i, void* handler) {
    
    idt[i].offset_low = GATE_OFFSET_LOW(handler);
    idt[i].offset_mid = GATE_OFFSET_MID(handler);
    idt[i].offset_high = GATE_OFFSET_HIGH(handler);
    idt[i].flags = GATE_FLAGS(1, 0, INTERRUPT_GATE);
    idt[i].segment = 8;
    
}
