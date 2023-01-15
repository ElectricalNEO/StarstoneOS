#pragma once
#include "../common.h"

#define SCANCODE_LSHIFT_PRESS 0x2a
#define SCANCODE_RSHIFT_PRESS 0x36

#define SCANCODE_LSHIFT_RELEASE 0xaa
#define SCANCODE_RSHIFT_RELEASE 0xb6

#define SCANCODE_CAPSLOCK_PRESS 0x3a
#define SCANCODE_CAPSLOCK_RELEASE 0xba

uint8_t shift;
uint8_t capslock;

uint8_t scancode_to_ascii[0xff];
uint8_t scancode_to_ascii_shift[0xff];
uint8_t scancode_to_ascii_capslock[0xff];
uint8_t scancode_to_ascii_capslock_shift[0xff];
