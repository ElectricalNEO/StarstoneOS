#pragma once
#include "memory.h"
#include "../initrd.h"

int init_page_frame_allocator(struct memory_map* memory_map, struct initrd* initrd);
uint64_t request_page_frame();
void free_page_frame(uint64_t addr);
