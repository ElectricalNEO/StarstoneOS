#pragma once
#include <stdarg.h>
#include "common.h"
#include "framebuffer.h"
#include "psf.h"

struct terminal {
    
    void (*clear)(struct terminal* term);
    void (*putc)(struct terminal* term, char ch);
    void (*puts)(struct terminal* term, char* str);
    void (*printf)(struct terminal* term, char* fmt, ...);
    void (*vprintf)(struct terminal* term, char* fmt, va_list args);
    void (*activate)(struct terminal* term);
    char (*getc)(struct terminal* term);
    char* (*gets)(struct terminal* term);
    void (*stdin_write)(struct terminal* term, char ch); // invoked by key press handler
    void (*scroll)(struct terminal* term);
    struct terminal_backend* backend;
    
};

struct terminal* create_terminal(struct framebuffer* framebuffer, struct psf_header* font);

void clear();
void putc(char ch);
void puts(char* str);
void printf(char* fmt, ...);
char getc();
char* gets();
void stdin_write(char ch); // invoked by key press handler
