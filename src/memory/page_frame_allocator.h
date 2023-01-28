#pragma once
#include "memory.h"
#include "../initrd.h"
#include "../framebuffer.h"

int init_page_frame_allocator(struct memory_map* memory_map, struct framebuffer* framebuffer, struct initrd* initrd);
uint64_t request_page_frame();
void free_page_frame(uint64_t addr);
uint64_t get_free_memory();
uint64_t get_used_memory();
