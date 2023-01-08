#include "interrupts.h"
#include "../text_renderer.h"
#include "../port.h"
#include "pic.h"

__attribute__((interrupt)) void int_ignore(struct interrupt_frame* frame) {
    
    UNUSED(frame);
    printf("Interrupt!\n");
    
}

__attribute__((interrupt)) void int_0eh(struct interrupt_frame* frame) {
    
    UNUSED(frame);
    printf("Page fault!\n");
    while(1);
    
}

__attribute__((interrupt)) void int_21h(struct interrupt_frame* frame) {
    
    UNUSED(frame);
    printf("0x%x\n", inb(0x60));
    pic_end_master();
    
}
