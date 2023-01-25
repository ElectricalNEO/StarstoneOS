#pragma once
#include "../common.h"

#define PIT_DATA_PORT 0x40
#define PIT_BASE_FREQUENCY 1193182

void pit_set_divisor(uint16_t divisor);
uint16_t pit_get_divisor();
uint64_t get_time_since_boot();

void tick();

void sleep(uint64_t ms);
