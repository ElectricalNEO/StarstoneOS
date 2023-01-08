#include "interrupts.h"
#include "../text_renderer.h"
#include "../port.h"
#include "pic.h"
#include "../input/keyboard.h"

__attribute__((interrupt)) void int_ignore(struct interrupt_frame* frame) {
    
    UNUSED(frame);
    
}

__attribute__((interrupt)) void int_0eh(struct interrupt_frame* frame) {
    
    UNUSED(frame);
    printf("Page fault!\n");
    while(1);
    
}

__attribute__((interrupt)) void int_21h(struct interrupt_frame* frame) {
    
    UNUSED(frame);
    key_buffer = scancode_to_ascii[inb(0x60)];
    pic_end_master();
    
}
