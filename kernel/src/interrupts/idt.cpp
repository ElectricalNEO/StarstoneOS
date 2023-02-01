#include <memory/memory.hpp>
#include <interrupts/idt.hpp>
#include <interrupts/pic.hpp>
#include <memory/PageTable.hpp>
#include <interrupts/interrupts.hpp>
#include <memory/PageFrameAllocator.hpp>

using namespace interrupts;

void IDT::GateDescriptor::setValues(uint8_t dpl, void* handler) {
	
	offsetLow = GATE_OFFSET_LOW(handler);
    offsetMid = GATE_OFFSET_MID(handler);
    offsetHigh = GATE_OFFSET_HIGH(handler);
    flags = GATE_FLAGS(1, dpl, INTERRUPT_GATE);
    segment = 8;
	
}

IDT::IDTR::IDTR(uint64_t offset) {
	
	this->size = 4095;
	this->offset = offset;
	
}

IDT::IDT(uint64_t virtualAddress) {
	
	descriptors = (GateDescriptor*)0;
	
	uint64_t idtPhys = memory::PageFrameAllocator::requestPageFrame();
    if(!idtPhys) return;
    if(memory::PageTable::mapPage(idtPhys, virtualAddress, PDE_FLAG_RW)) return;
    descriptors = (GateDescriptor*)virtualAddress;
	
	memory::memset((void*)virtualAddress, 4096, 0);
	
    for(uint16_t i = 0; i < 256; i++) {
        
        descriptors[i].setValues(0, (void*)int_ignore);
        
    }
    
	descriptors[0x6].setValues(0, (void*)int_06);
	descriptors[0x8].setValues(0, (void*)int_08);
	descriptors[0xd].setValues(0, (void*)int_0d);
	descriptors[0xe].setValues(0, (void*)int_0e);
	descriptors[0x20].setValues(0, (void*)int_20);
	descriptors[0x80].setValues(3, (void*)int_80);
	descriptors[0x81].setValues(0, (void*)int_81);
    
    IDTR idtr(virtualAddress);
    
    asm("lidt %0" : : "m" (idtr));
	PIC::remap();
	
}

uint8_t IDT::isGood() {
	
	if(!(uint64_t)descriptors) return 0;
	return 1;
	
}

void IDT::activate() {
	
	asm("sti");
	PIC::setSlaveMask(0b11111111);
	PIC::setMasterMask(0b11111110);
	
}
