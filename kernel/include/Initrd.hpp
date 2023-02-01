#pragma once
#include <common.hpp>

struct Initrd {
	
	void* address;
	uint32_t size;
	
}__attribute__((packed));
