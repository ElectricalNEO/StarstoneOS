#pragma once
#include "../common.h"

struct interrupt_frame;
__attribute__((interrupt)) void int_ignore(struct interrupt_frame* frame);
__attribute__((interrupt)) void int_06h(struct interrupt_frame* frame); // Invalid Opcode
__attribute__((interrupt)) void int_08h(struct interrupt_frame* frame); // Double Fault
__attribute__((interrupt)) void int_0dh(struct interrupt_frame* frame); // General Protection Fault
__attribute__((interrupt)) void int_0eh(struct interrupt_frame* frame); // Page Fault
__attribute__((interrupt)) void int_20h(struct interrupt_frame* frame);
__attribute__((interrupt)) void int_21h(struct interrupt_frame* frame);
__attribute__((interrupt)) void int_80h(struct interrupt_frame* frame);
