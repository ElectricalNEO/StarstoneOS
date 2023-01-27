#include "interrupts.h"
#include "../terminal.h"
#include "../port.h"
#include "pic.h"
#include "../input/keyboard.h"
#include "../text_renderer.h"
#include "../scheduling/pit.h"
#include "../scheduling/multitasking.h"
#include "../memory/memory.h"

__attribute__((interrupt)) void int_ignore(struct interrupt_frame* frame) {
    
    UNUSED(frame);
    
}

__attribute__((interrupt)) void int_06h(struct interrupt_frame* frame) {
    
    UNUSED(frame);
	clear();
    puts("Invalid opcode!\n");
    while(1);
    
}

__attribute__((interrupt)) void int_08h(struct interrupt_frame* frame) {
    
    UNUSED(frame);
	clear();
    puts("Double fault!\n");
    while(1);
    
}

__attribute__((interrupt)) void int_0dh(struct interrupt_frame* frame) {
    
    UNUSED(frame);
	clear();
    puts("General Protection fault!\n");
    while(1);
    
}

__attribute__((interrupt)) void int_0eh(struct interrupt_frame* frame) {
    
    UNUSED(frame);
	//clear();
    puts("Page fault!\n");
    while(1);
    
}

__attribute__((interrupt)) void int_21h(struct interrupt_frame* frame) {
    
    UNUSED(frame);
    uint8_t scancode = inb(0x60);
    if(scancode == SCANCODE_LSHIFT_PRESS || scancode == SCANCODE_RSHIFT_PRESS) shift = 1;
    if(scancode == SCANCODE_LSHIFT_RELEASE || scancode == SCANCODE_RSHIFT_RELEASE) shift = 0;
    if(scancode == SCANCODE_CAPSLOCK_PRESS) capslock = !capslock;
    else {
        
        if(!shift && !capslock) stdin_write(scancode_to_ascii[scancode]);
        else if(shift && !capslock) stdin_write(scancode_to_ascii_shift[scancode]);
        else if(!shift && capslock) stdin_write(scancode_to_ascii_capslock[scancode]);
        else stdin_write(scancode_to_ascii_capslock_shift[scancode]);
        
    }
    
    pic_end_master();
    
}
