#pragma once
#include "common.h"
#include "framebuffer.h"
#include "psf.h"

void init_text_renderer(struct framebuffer* framebuffer, struct psf_header* font);
void tr_putc(char ch);
void tr_puts(char* str);
