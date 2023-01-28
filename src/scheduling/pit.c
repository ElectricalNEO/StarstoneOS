#include "pit.h"
#include "../hardware/port.h"

uint16_t pit_divisor;
uint64_t time_since_boot = 0;

void pit_set_divisor(uint16_t divisor) {
	
	pit_divisor = divisor;
	
	outb(PIT_DATA_PORT, (uint8_t)(divisor & 0xff));
	io_wait();
	outb(PIT_DATA_PORT, (uint8_t)((divisor & 0xff00) >> 8));
	
}

uint16_t pit_get_divisor() {
	
	return pit_divisor;
	
}

uint64_t get_time_since_boot() {
	
	return time_since_boot;
	
}

void tick() {
	
	time_since_boot += (uint64_t)((double)1 / (PIT_BASE_FREQUENCY / (double)pit_divisor) * 1000);
	
}

void sleep(uint64_t ms) {
	
	uint64_t start = time_since_boot;
	while(time_since_boot < start + ms) asm("hlt");
	
}
