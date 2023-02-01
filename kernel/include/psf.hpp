#pragma once
#include <common.hpp>

#define PSF2_MAGIC 0x864ab572

struct PSF2_Header {
    uint32_t magic;
    uint32_t version;
    uint32_t headerSize;
    uint32_t flags;
    uint32_t glyphs;
    uint32_t bytesPerGlyph;
    uint32_t height;
    uint32_t width;
}__attribute__((packed));
