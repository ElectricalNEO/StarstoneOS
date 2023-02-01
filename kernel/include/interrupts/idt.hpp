#pragma once
#include <common.hpp>

#define GATE_OFFSET_LOW(offset) ((uint64_t)offset & 0xffff)
#define GATE_OFFSET_MID(offset) (((uint64_t)offset >> 16) & 0xffff)
#define GATE_OFFSET_HIGH(offset) (((uint64_t)offset >> 32) & 0xffffffff)
#define GATE_FLAGS(p, dpl, type) (p << 7) | (dpl << 5) | type

#define INTERRUPT_GATE 0xe
#define TRAP_GATE 0xe

namespace interrupts {
	
	class IDT {
		
		class GateDescriptor {
			
			uint16_t offsetLow;
			uint16_t segment;
			uint8_t ist;
			uint8_t flags;
			uint16_t offsetMid;
			uint32_t offsetHigh;
			uint32_t reserved;
			
		public:
			void setValues(uint8_t dpl, void* handler);
			
		}__attribute__((packed));
		
		class IDTR {
			
			uint16_t size;
    		uint64_t offset;
		public:
			IDTR(uint64_t offset);
			
		}__attribute__((packed));
		
		GateDescriptor* descriptors;
		
	public:
		IDT(uint64_t virtualAddress);
		uint8_t isGood();
		void activate();
		
	};
	
}
