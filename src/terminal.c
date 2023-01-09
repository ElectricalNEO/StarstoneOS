#include <stdarg.h>
#include "terminal.h"
#include "memory/heap.h"
#include "memory/memory.h"
#include "string.h"

struct terminal_backend {
    
    struct framebuffer* framebuffer;
    uint64_t terminal_buffer;
    struct psf_header* font;
    uint16_t ch_x;
    uint16_t ch_y;
    
};

struct terminal* active_terminal = 0;
char _itoa_buf[21];

void terminal_clear(struct terminal* term) {
    
    for(uint32_t y = 0; y < term->backend->framebuffer->height; y++)
        for(uint32_t x = 0; x < term->backend->framebuffer->width; x++)
            *(uint32_t*)(term->backend->terminal_buffer + y * term->backend->framebuffer->pitch + x * term->backend->framebuffer->bpp / 8) = 0;
    
    if(active_terminal == term) {
        for(uint32_t y = 0; y < term->backend->framebuffer->height; y++)
            for(uint32_t x = 0; x < term->backend->framebuffer->width; x++)
                *(uint32_t*)(term->backend->framebuffer->address + y * term->backend->framebuffer->pitch + x * term->backend->framebuffer->bpp / 8) = 0;
    }
    
    term->backend->ch_x = 0;
    term->backend->ch_y = 0;
    
}

void terminal_putc(struct terminal* term, char ch) {
    
    if(ch == '\n') {
        
        term->backend->ch_x = 0;
        term->backend->ch_y += term->backend->font->height;
        return;
        
    }
    
    uint32_t offset = term->backend->font->header_size + ch * term->backend->font->bytes_per_glyph;
    uint8_t* glyph_ptr = (uint8_t*)((uint64_t)term->backend->font + offset);
    uint8_t bit = 0b10000000;
    
    for(uint32_t y = 0; y < term->backend->font->height; y++) {
        for(uint32_t x = 0; x < term->backend->font->width; x++) {
            if(*glyph_ptr & bit) {
                *(uint32_t*)((uint64_t)term->backend->terminal_buffer + (y + term->backend->ch_y) * term->backend->framebuffer->pitch + (x + term->backend->ch_x) * term->backend->framebuffer->bpp / 8) = 0x00ff00;
                if(active_terminal == term)
                    *(uint32_t*)((uint64_t)term->backend->framebuffer->address + (y + term->backend->ch_y) * term->backend->framebuffer->pitch + (x + term->backend->ch_x) * term->backend->framebuffer->bpp / 8) = 0x00ff00;
            }
            bit >>= 1;
            if(!bit) {
                bit = 0b10000000;
                glyph_ptr++;
            }
        }
        glyph_ptr++;
        bit = 0b10000000;
    }
    term->backend->ch_x += term->backend->font->width;
    
    if(term->backend->ch_x + term->backend->font->height >= term->backend->framebuffer->width - term->backend->font->height / 2) {
        
        term->backend->ch_x = 0;
        term->backend->ch_y += term->backend->font->height;
        
    }
    
}

void terminal_printf(struct terminal* term, char* fmt, ...) {
    
    va_list args;
    va_start(args, fmt);
    
    while(*fmt) {
        
        if(*fmt == '%') {
            
            fmt++;
            switch(*fmt) {
                
            case 'd':
                term->puts(term, itoa(va_arg(args, uint64_t), _itoa_buf, 10));
                break;
            case 's':
                term->puts(term, va_arg(args, char*));
                break;
            case 'x':
                term->puts(term, itoa(va_arg(args, uint64_t), _itoa_buf, 16));
                break;
            case 'c':
                term->putc(term, va_arg(args, int));
                break;
            default:
                term->putc(term, '%');
                
            }
            
        } else term->putc(term, *fmt);
        
        fmt++;
        
    }
    
    va_end(args);
    
}

void terminal_activate(struct terminal* term) {
    
    memcpy((void*)term->backend->terminal_buffer, (void*)term->backend->framebuffer->address, term->backend->framebuffer->pitch * term->backend->framebuffer->height);
    active_terminal = term;
    
}

void terminal_puts(struct terminal* term, char* str) {
    
    while(*str) {
        
        term->putc(term, *str);
        str++;
        
    }
    
}

struct terminal* create_terminal(struct framebuffer* framebuffer, struct psf_header* font) {
    
    if(!framebuffer || !font || font->magic != PSF_FONT_MAGIC) return 0;
    
    struct terminal* term = malloc(sizeof(struct terminal));
    if(!term) return 0;
    
    term->backend = malloc(sizeof(struct terminal_backend));
    if(!term->backend) {
        
        free(term);
        return 0;
        
    }
    term->backend->terminal_buffer = (uint64_t)malloc(framebuffer->pitch * framebuffer->height);
    if(!term->backend->terminal_buffer) {
        
        free(term->backend);
        free(term);
        return 0;
        
    }
    
    if(!active_terminal) active_terminal = term;
    
    term->clear = terminal_clear;
    term->putc = terminal_putc;
    term->puts = terminal_puts;
    term->printf = terminal_printf;
    term->activate = terminal_activate;
    
    term->backend->framebuffer = framebuffer;
    term->backend->font = font;
    term->backend->ch_x = 0;
    term->backend->ch_y = 0;
    
    term->clear(term);
    
    return term;
    
}

void clear() {
    
    terminal_clear(active_terminal);
    
}

void putc(char ch) {
    
    terminal_putc(active_terminal, ch);
    
}

void puts(char* str) {
    
    terminal_puts(active_terminal, str);
    
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
