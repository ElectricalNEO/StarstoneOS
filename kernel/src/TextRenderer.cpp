#include <TextRenderer.hpp>

Framebuffer* TextRenderer::framebuffer = 0;
PSF2_Header* TextRenderer::font;
uint32_t TextRenderer::x, TextRenderer::y;

void TextRenderer::init(Framebuffer* framebuffer, PSF2_Header* font) {
	
	TextRenderer::framebuffer = framebuffer;
	TextRenderer::font = font;
	clear();
	
}

void TextRenderer::clear() {
	
	if(!framebuffer) return;
	for(uint32_t y = 0; y < framebuffer->height; y++)
		for(uint32_t x = 0; x < framebuffer->width; x++)
			*(uint32_t*)(framebuffer->address + y * framebuffer->pitch + x * (framebuffer->bpp / 8)) = 0x0000ff;
	
	x = 0;
	y = 0;
	
}

void TextRenderer::putc(char ch) {
	
	if(!framebuffer) return;
	
	if(ch == '\n') {
        
        x = 0;
        y += font->height;
        return;
        
    }
    
    uint32_t offset = font->headerSize + ch * font->bytesPerGlyph;
    uint8_t* glyphPtr = (uint8_t*)((uint64_t)font + offset);
    uint8_t bit = 0b10000000;
    
    for(uint32_t y = 0; y < font->height; y++) {
        for(uint32_t x = 0; x < font->width; x++) {
            if(*glyphPtr & bit) *(uint32_t*)(framebuffer->address + (y + TextRenderer::y) * framebuffer->pitch + (x + TextRenderer::x) * framebuffer->bpp / 8) = 0xffffff;
            bit >>= 1;
            if(!bit) {
                bit = 0b10000000;
                glyphPtr++;
            }
        }
        glyphPtr++;
        bit = 0b10000000;
    }
    x += font->width;
    
    if(x + font->height >= framebuffer->width - font->height / 2) {
        
        x = 0;
        y += font->height;
        
    }
	
}

void TextRenderer::puts(char* str) {
	
	if(!framebuffer) return;
	
	while(*str) putc(*str++);
	
}
