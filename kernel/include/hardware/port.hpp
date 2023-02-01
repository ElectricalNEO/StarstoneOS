#pragma once
#include <common.hpp>

namespace hardware {
	
	class PortIO {
		
		PortIO();
	public:
		static void outb(uint16_t port, uint8_t value);
		static uint8_t inb(uint16_t port);
		static void io_wait();
		
	};
	
}
