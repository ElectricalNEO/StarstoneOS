#pragma once
#include "../common.h"

#define GATE_OFFSET_LOW(offset) ((uint64_t)offset & 0xffff)
#define GATE_OFFSET_MID(offset) (((uint64_t)offset >> 16) & 0xffff)
#define GATE_OFFSET_HIGH(offset) (((uint64_t)offset >> 32) & 0xffffffff)
#define GATE_FLAGS(p, dpl, type) (p << 7) | (dpl << 5) | type

#define INTERRUPT_GATE 0xe
#define TRAP_GATE 0xe

#define IDT_ADDRESS 0xffffffffbffff000

struct gate_descriptor {
    
    uint16_t offset_low;
    uint16_t segment;
    uint8_t ist;
    uint8_t flags;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
    
}__attribute__((packed));

struct idtr {
    
    uint16_t size;
    uint64_t offset;
    
}__attribute__((packed));

uint8_t init_idt();
void set_interrupt_gate(uint8_t i, uint8_t dpl, void* handler);
void activate_interrupts();
