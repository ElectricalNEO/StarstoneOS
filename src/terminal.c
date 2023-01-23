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
    volatile char stdin_buffer;
    
};

struct terminal* active_terminal = 0;
struct terminal_list_node terminal_list = {
	
	.data = 0,
	.next = 0
	
};
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
    term->backend->stdin_buffer = 0;
    
}

void terminal_scroll(struct terminal* term) {
    
    memcpy((void*)(term->backend->terminal_buffer + term->backend->framebuffer->pitch * term->backend->font->height), (void*)term->backend->terminal_buffer, term->backend->framebuffer->pitch * (term->backend->framebuffer->height - term->backend->font->height));
    memset((void*)(term->backend->terminal_buffer + term->backend->framebuffer->pitch * (term->backend->framebuffer->height - term->backend->font->height)), term->backend->framebuffer->pitch * term->backend->font->height, 0);
    term->backend->ch_y -= term->backend->font->height;
    if(active_terminal == term)
        memcpy((void*)term->backend->terminal_buffer, (void*)term->backend->framebuffer->address, term->backend->framebuffer->pitch * term->backend->framebuffer->height);
    
}

void terminal_putc(struct terminal* term, char ch) {
    
    if(ch == '\n') {
        
        term->backend->ch_x = 0;
        term->backend->ch_y += term->backend->font->height;
        if(term->backend->ch_y + term->backend->font->height >= term->backend->framebuffer->height) terminal_scroll(term);
        return;
        
    } else if(ch == '\b') {
        
        if(term->backend->ch_x) term->backend->ch_x -= term->backend->font->width;
        else if(term->backend->ch_y) {
            
            term->backend->ch_x = term->backend->framebuffer->width - term->backend->font->width;
            term->backend->ch_y -= term->backend->font->height;
            if(term->backend->ch_y + term->backend->font->height >= term->backend->framebuffer->height) terminal_scroll(term);
            
        } else return;
        
        for(uint32_t y = 0; y < term->backend->font->height; y++) {
            for(uint32_t x = 0; x < term->backend->font->width; x++) {
                *(uint32_t*)((uint64_t)term->backend->terminal_buffer + (y + term->backend->ch_y) * term->backend->framebuffer->pitch + (x + term->backend->ch_x) * term->backend->framebuffer->bpp / 8) = 0;
                if(active_terminal == term)
                    *(uint32_t*)((uint64_t)term->backend->framebuffer->address + (y + term->backend->ch_y) * term->backend->framebuffer->pitch + (x + term->backend->ch_x) * term->backend->framebuffer->bpp / 8) = 0;
                
            }
        }
        return;
        
    } else if(ch == '\t') {
        
        term->backend->ch_x += 4 * term->backend->font->width - term->backend->ch_x % (4 * term->backend->font->width);
        
        if(term->backend->ch_x + term->backend->font->width > term->backend->framebuffer->width) {
            
            term->backend->ch_x = 0;
            term->backend->ch_y += term->backend->font->height;
            if(term->backend->ch_y + term->backend->font->height >= term->backend->framebuffer->height) terminal_scroll(term);
            
        }
        
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
    
    if(term->backend->ch_x + term->backend->font->width > term->backend->framebuffer->width) {
        
        term->backend->ch_x = 0;
        term->backend->ch_y += term->backend->font->height;
        if(term->backend->ch_y + term->backend->font->height >= term->backend->framebuffer->height) terminal_scroll(term);
        
    }
    
}

void terminal_printf(struct terminal* term, char* fmt, ...) {
    
    va_list args;
    va_start(args, fmt);
    
    term->vprintf(term, fmt, args);
    
    va_end(args);
    
}

void terminal_vprintf(struct terminal* term, char* fmt, va_list args) {
    
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

char terminal_getc(struct terminal* term) {
    
    while(!term->backend->stdin_buffer);
    char ret = term->backend->stdin_buffer;
    term->backend->stdin_buffer = 0;
    return ret;
    
}

char* terminal_gets(struct terminal* term) {
    
    char input[255];
    uint8_t len = 0;
    
    while(1) {
        
        char ch = term->getc(term);
        if(ch == '\b') {
            
            if(len) {
                input[len] = 0;
                len--;
                term->putc(term, '\b');
            }
            
        } else if (ch == '\n') {
            
            term->putc(term, '\n');
            input[len] = 0;
            
            void* ret = malloc(len + 1);
            if(!ret) return 0;
            memcpy(input, ret, len + 1);
            return ret;
            
        } else if(len < 254) {
            
            input[len] = ch;
            len++;
            term->putc(term, ch);
            
        }
        
    }
    
}

void terminal_stdin_write(struct terminal* term, char ch) {
    
    term->backend->stdin_buffer = ch;
    
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
    
	struct terminal_list_node* node = &terminal_list;
	while(node->data && node->next) {
		
		node = node->next;
		
	}
	
	if(node->data) {
		
		node->next = malloc(sizeof(struct terminal_list_node));
		if(!node->next) {
			
			free((void*)term->backend->terminal_buffer);
			free(term->backend);
			free(term);
			return 0;
			
		}
		node = node->next;
		node->next = 0;
		
	}
	
	node->data = term;
	
    if(!active_terminal) active_terminal = term;
    
    term->clear = terminal_clear;
    term->putc = terminal_putc;
    term->puts = terminal_puts;
    term->printf = terminal_printf;
    term->vprintf = terminal_vprintf;
    term->activate = terminal_activate;
    term->getc = terminal_getc;
    term->gets = terminal_gets;
    term->stdin_write = terminal_stdin_write;
    
    term->backend->framebuffer = framebuffer;
    term->backend->font = font;
    term->backend->ch_x = 0;
    term->backend->ch_y = 0;
    
    term->clear(term);
    
    return term;
    
}

struct terminal* get_terminal_by_index(uint8_t index) {
	
	struct terminal_list_node* node = &terminal_list;
	for(uint8_t i = 0; i < index; i++) {
		
		if(!node->next) return 0;
		node = node->next;
		
	}
	return node->data;
	
}

uint8_t get_index_of_terminal(struct terminal* terminal) {
	
	uint8_t index = 0;
	struct terminal_list_node* node = &terminal_list;
	while(node->next && node->data != terminal) {
		
		if(node->data) index++;
		node = node->next;
		
	}
	return index;
	
}

void stdin_write(char ch) {
    
    if(!active_terminal) return;
    
	if(ch == 0x11) {
		
		uint8_t index = get_index_of_terminal(active_terminal);
		if(!index) return;
		struct terminal* term = get_terminal_by_index(index-1);
		if(term) term->activate(term);
		
		return;
		
	} else if(ch == 0x12) {
		
		uint8_t index = get_index_of_terminal(active_terminal);
		struct terminal* term = get_terminal_by_index(index+1);
		if(term) term->activate(term);
		
		return;
		
	}
	
    active_terminal->stdin_write(active_terminal, ch);
    
}
