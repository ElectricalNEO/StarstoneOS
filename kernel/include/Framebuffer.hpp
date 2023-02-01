#pragma once
#include <common.hpp>

struct Framebuffer {
	
	uint64_t address;
	uint32_t pitch;
	uint32_t width;
	uint32_t height;
	uint8_t bpp;
	
}__attribute__((packed));
