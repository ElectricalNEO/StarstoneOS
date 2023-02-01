#include <hardware/port.hpp>

using namespace hardware;

void PortIO::outb(uint16_t port, uint8_t value) {
	
	asm volatile ("out %0, %1" : : "Nd"(port), "a"(value));
	
}

uint8_t PortIO::inb(uint16_t port) {
	
	uint8_t value;
    asm volatile ("in %0, %1" : "=a"(value) : "Nd"(port));
    return value;
	
}

void PortIO::io_wait() {
	
	asm volatile ("outb 0x80, al" : : "a"(0));
	
}
