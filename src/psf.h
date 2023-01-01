#pragma once
#include "common.h"

#define PSF_FONT_MAGIC 0x864ab572

struct psf_header {
    uint32_t magic;
    uint32_t version;
    uint32_t header_size;
    uint32_t flags;
    uint32_t glyphs;
    uint32_t bytes_per_glyph;
    uint32_t height;
    uint32_t width;
}__attribute__((packed));
