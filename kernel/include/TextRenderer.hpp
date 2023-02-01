#pragma once
#include <psf.hpp>
#include <common.hpp>
#include <Framebuffer.hpp>

class TextRenderer {
	
	TextRenderer();
	static Framebuffer* framebuffer;
	static PSF2_Header* font;
	static uint32_t x, y;
	
public:
	
	static void init(Framebuffer* framebuffer, PSF2_Header* font);
	static void clear();
	static void putc(char ch);
	static void puts(char* str);
	
};
