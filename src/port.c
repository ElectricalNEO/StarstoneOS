#include "port.h"

void outb(uint16_t port, uint8_t val) {
    
    asm volatile ("out %0, %1" : : "Nd"(port), "a"(val));
    
}

uint8_t inb(uint16_t port) {
    
    uint8_t val;
    asm volatile ("in %0, %1" : "=a"(val) : "Nd"(port));
    return val;
    
}

void outd(uint16_t port, uint32_t val) {
    
    asm volatile ("out %0, %1" : : "Nd"(port), "a"(val));
    
}

uint32_t ind(uint16_t port) {
    
    uint32_t val;
    asm volatile ("in %0, %1" : "=a"(val) : "Nd"(port));
    return val;
    
}

void io_wait() {
    
    asm volatile ("outb 0x80, al" : : "a"(0));
    
}
