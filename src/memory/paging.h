#pragma once
#include "memory.h"
#include "bitmap.h"

#define PML4_RECURSIVE 508

#define CONSTRUCT_VIRTUAL(pml4_i, pdpt_i, pd_i, pt_i) ((pml4_i & 0b100000000 ? 0xffffULL << (12 + 9 * 4) : 0) | ((uint64_t)pml4_i << (12 + 9 * 3))  | ((uint64_t)pdpt_i << (12 + 9 * 2))  | ((uint64_t)pd_i << (12 + 9 * 1)) | ((uint64_t)pt_i << 12))

#define PDE_GET_FLAG(pde, flag) (pde & (1 << flag))

#define PDE_FLAG_PRESENT 1 << 0
#define PDE_FLAG_RW 1 << 1

void map_page(uint64_t physical_address, uint64_t virtual_address);
