#include <hardware/pit.hpp>
#include <hardware/port.hpp>

using namespace hardware;

uint16_t PIT::divisor;

void PIT::setDivisor(uint16_t divisor) {
	
	divisor = divisor;
	
	PortIO::outb(PIT_DATA_PORT, (uint8_t)(divisor & 0xff));
	PortIO::io_wait();
	PortIO::outb(PIT_DATA_PORT, (uint8_t)((divisor & 0xff00) >> 8));
	
}

uint16_t PIT::getDivisor() {
	
	return divisor;
	
}
