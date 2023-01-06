#pragma once
#include "common.h"

#define BITMAP_SET(bitmap, bit) *(uint8_t*)((uint64_t)bitmap + bit / 8) |= 0b10000000 >> (bit % 8)
#define BITMAP_CLEAR(bitmap, bit) *(uint8_t*)((uint64_t)bitmap + bit / 8) &= ~(0b10000000 >> (bit % 8))
#define BITMAP_GET(bitmap, bit) (*(uint8_t*)((uint64_t)bitmap + bit / 8) & 0b10000000 >> (bit % 8))
