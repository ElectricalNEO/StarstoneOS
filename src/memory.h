#pragma once
#include "common.h"

#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_MEMORY_RESERVED 2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT_MEMORY_NVS 4
#define MULTIBOOT_MEMORY_BADRAM 5

char* memory_type_strings[6];

struct memory_map_entry {
    
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
    
}__attribute__((packed));

struct memory_map {
    
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct memory_map_entry entries[0];
    
}__attribute__((packed));

uint64_t get_memory_size(struct memory_map* memory_map);

void memcpy(void* src, void* dst, uint64_t n);
void memset(void* s, uint64_t n, uint8_t c);
