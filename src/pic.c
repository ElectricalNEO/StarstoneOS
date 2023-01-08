#include "pic.h"
#include "port.h"

void remap_pic() {
    
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);
    
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, PIC1_OFFSET);
    io_wait();
    outb(PIC2_DATA, PIC2_OFFSET);
    io_wait();
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();
    
    outb(PIC1_DATA, mask1);
    io_wait();
    outb(PIC2_DATA, mask2);
    
}

void pic_master_set_mask(uint8_t mask) {
    
    outb(PIC1_DATA, mask);
    
}

void pic_slave_set_mask(uint8_t mask) {
    
    outb(PIC2_DATA, mask);
    
}

void pic_end_master() {
    
    outb(PIC1_COMMAND, PIC_EOI);
    
}

void pic_end_slave() {
    
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
    
}
