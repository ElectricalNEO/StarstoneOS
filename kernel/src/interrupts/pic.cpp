#include <hardware/port.hpp>
#include <interrupts/pic.hpp>

using namespace interrupts;

void PIC::remap() {
	
	uint8_t mask1 = hardware::PortIO::inb(PIC1_DATA);
    uint8_t mask2 = hardware::PortIO::inb(PIC2_DATA);
    
    hardware::PortIO::outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    hardware::PortIO::io_wait();
    hardware::PortIO::outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    hardware::PortIO::io_wait();
    hardware::PortIO::outb(PIC1_DATA, PIC1_OFFSET);
    hardware::PortIO::io_wait();
    hardware::PortIO::outb(PIC2_DATA, PIC2_OFFSET);
    hardware::PortIO::io_wait();
    hardware::PortIO::outb(PIC1_DATA, 4);
    hardware::PortIO::io_wait();
    hardware::PortIO::outb(PIC2_DATA, 2);
    hardware::PortIO::io_wait();
    hardware::PortIO::outb(PIC1_DATA, ICW4_8086);
    hardware::PortIO::io_wait();
    hardware::PortIO::outb(PIC2_DATA, ICW4_8086);
    hardware::PortIO::io_wait();
    
    hardware::PortIO::outb(PIC1_DATA, mask1);
    hardware::PortIO::io_wait();
    hardware::PortIO::outb(PIC2_DATA, mask2);
	
}

void PIC::setMasterMask(uint8_t mask) {
	
	hardware::PortIO::outb(PIC1_DATA, mask);
	
}

void PIC::setSlaveMask(uint8_t mask) {
	
	hardware::PortIO::outb(PIC2_DATA, mask);
	
}

void PIC::endMaster() {
	
	hardware::PortIO::outb(PIC1_COMMAND, PIC_EOI);
	
}

void PIC::endSlave() {
	
	hardware::PortIO::outb(PIC2_COMMAND, PIC_EOI);
    hardware::PortIO::outb(PIC1_COMMAND, PIC_EOI);
	
}
