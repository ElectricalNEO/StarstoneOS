#pragma once
#include "common.h"
#include "framebuffer.h"
#include "psf.h"

int init_text_renderer(struct framebuffer* framebuffer, struct psf_header* font);
void clear();
void putc(char ch);
void puts(char* str);
