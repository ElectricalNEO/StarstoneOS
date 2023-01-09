#pragma once
#include "common.h"
#include "framebuffer.h"
#include "psf.h"

struct terminal {
    
    void (*clear)(struct terminal* term);
    void (*putc)(struct terminal* term, char ch);
    void (*puts)(struct terminal* term, char* str);
    void (*printf)(struct terminal* term, char* fmt, ...);
    void (*activate)(struct terminal* term);
    struct terminal_backend* backend;
    
};

struct terminal* create_terminal(struct framebuffer* framebuffer, struct psf_header* font);

void clear();
void putc(char ch);
void puts(char* str);
void printf(char* fmt, ...);
