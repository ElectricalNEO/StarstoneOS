#pragma once
#include "../common.h"

struct interrupt_frame;
__attribute__((interrupt)) void int_ignore(struct interrupt_frame* frame);
__attribute__((interrupt)) void int_0eh(struct interrupt_frame* frame);
__attribute__((interrupt)) void int_20h(struct interrupt_frame* frame);
__attribute__((interrupt)) void int_21h(struct interrupt_frame* frame);
