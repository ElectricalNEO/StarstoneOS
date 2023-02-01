#pragma once
#include <common.hpp>

namespace interrupts {
	
	__attribute__((interrupt)) void int_ignore(void* frame);
	__attribute__((interrupt)) void int_06(void* frame); // Invalid Opcode
	__attribute__((interrupt)) void int_08(void* frame); // Double Fault
	__attribute__((interrupt)) void int_0d(void* frame); // General Protection Fault
	__attribute__((interrupt)) void int_0e(void* frame); // Page Fault
	extern "C" void int_20(); // PIT + Task switch
	extern "C" void int_80(); // Syscall
	extern "C" void int_81(); // Task switch
	
}
