#pragma once
#include "common.h"

struct framebuffer {
    
    uint64_t address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t type;
    
}__attribute__((packed));
