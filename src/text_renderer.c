#include "text_renderer.h"
#include <stdarg.h>
#include "string.h"
#include "memory/memory.h"

struct psf_header* _font;
uint32_t _chx, _chy;

void init_text_renderer(struct psf_header* font) {
    
    _font = font;
    
    if(!font || font->magic != PSF_FONT_MAGIC) {
        
        for(uint32_t y = 0; y < _framebuffer->height; y++)
            for(uint32_t x = 0; x < _framebuffer->width; x++)
                *(uint32_t*)(_framebuffer->address + y * _framebuffer->pitch + x * _framebuffer->bpp / 8) = 0xff0000;
        while(1);
        
    }
    
	clear();
}

void putc(char ch) {
    
    if(ch == '\n') {
        
        _chx = 0;
        _chy += _font->height;
        return;
        
    }
    
    uint32_t offset = _font->header_size + ch * _font->bytes_per_glyph;
    uint8_t* glyph_ptr = (uint8_t*)((uint64_t)_font + offset);
    uint8_t bit = 0b10000000;
    
    for(uint32_t y = 0; y < _font->height; y++) {
        for(uint32_t x = 0; x < _font->width; x++) {
            if(*glyph_ptr & bit) *(uint32_t*)(_framebuffer->address + (y + _chy) * _framebuffer->pitch + (x + _chx) * _framebuffer->bpp / 8) = 0xffffff;
            bit >>= 1;
            if(!bit) {
                bit = 0b10000000;
                glyph_ptr++;
            }
        }
        glyph_ptr++;
        bit = 0b10000000;
    }
    _chx += _font->width;
    
    if(_chx + _font->height >= _framebuffer->width - _font->height / 2) {
        
        _chx = 0;
        _chy += _font->height;
        
    }
    
}

void puts(char* str) {
    
    while(*str) {
        
        putc(*str);
        str++;
        
    }
    
}

void clear() {
	
	for(uint32_t y = 0; y < _framebuffer->height; y++)
        for(uint32_t x = 0; x < _framebuffer->width; x++)
            *(uint32_t*)(_framebuffer->address + y * _framebuffer->pitch + x * _framebuffer->bpp / 8) = 0x0000ff;
    
    _chx = 0;
    _chy = 0;
	
}
