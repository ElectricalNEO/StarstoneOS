#pragma once
#include "../common.h"

uint8_t scancode_to_ascii[0xff];
volatile uint8_t key_buffer;

uint8_t getc();
