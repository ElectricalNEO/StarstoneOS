#pragma once
#include <common.hpp>

#define PIT_DATA_PORT 0x40
#define PIT_BASE_FREQUENCY 1193182

namespace hardware {
	
	class PIT {
		
		static uint16_t divisor;
		PIT();
	public:
		static void setDivisor(uint16_t divisor);
		static uint16_t getDivisor();
		
	};
	
}
