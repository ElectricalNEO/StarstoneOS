#pragma once
#include "common.h"

struct initrd {
    
    uint64_t address;
    uint32_t size;
    
}__attribute__((packed));
