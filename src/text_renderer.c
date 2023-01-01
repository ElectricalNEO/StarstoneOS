#include "text_renderer.h"
#include "stdarg.h"
#include "string.h"

struct framebuffer* _framebuffer;
struct psf_header* _font;
uint32_t _chx, _chy;
char _itoa_buf[21];

int init_text_renderer(struct framebuffer* framebuffer, struct psf_header* font) {
    
    _framebuffer = framebuffer;
    _font = font;
    
    if(!font || font->magic != PSF_FONT_MAGIC) {
        
        for(uint32_t y = 0; y < framebuffer->height; y++)
            for(uint32_t x = 0; x < framebuffer->width; x++)
                *(uint64_t*)(framebuffer->address + y * framebuffer->pitch + x * framebuffer->bpp / 8) = 0xff0000;
        return 0;
        
    }
    
    clear();
    
    return 1;
    
}

void clear() {
    
    for(uint32_t y = 0; y < _framebuffer->height; y++)
        for(uint32_t x = 0; x < _framebuffer->width; x++)
            *(uint64_t*)(_framebuffer->address + y * _framebuffer->pitch + x * _framebuffer->bpp / 8) = 0x0000ff;
    _chx = _font->height / 2;
    _chy = _font->height / 2;
    
}

void putc(char ch) {
    
    if(ch == '\n') {
        
        _chx = _font->height / 2;
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
        
        _chx = _font->height / 2;
        _chy += _font->height;
        
    }
    
}

void puts(char* str) {
    
    while(*str) {
        
        putc(*str);
        str++;
        
    }
    
}

void printf(char* fmt, ...) {
    
    va_list args;
    va_start(args, fmt);
    
    while(*fmt) {
        
        if(*fmt == '%') {
            
            fmt++;
            switch(*fmt) {
                
            case 'd':
                puts(itoa(va_arg(args, uint64_t), _itoa_buf, 10));
                break;
            case 's':
                puts(va_arg(args, char*));
                break;
            case 'x':
                puts(itoa(va_arg(args, uint64_t), _itoa_buf, 16));
                break;
            case 'c':
                putc(va_arg(args, int));
                break;
            default:
                putc('%');
                
            }
            
        } else putc(*fmt);
        
        fmt++;
        
    }
    
    va_end(args);
    
}
