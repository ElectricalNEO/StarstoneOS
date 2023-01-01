#include "common.h"
#include "framebuffer.h"
#include "initrd.h"
#include "tar.h"
#include "psf.h"
#include "text_renderer.h"
#include "string.h"

int main(struct framebuffer* framebuffer, struct initrd* initrd) {
    
    struct psf_header* font = tar_open_file((void*)(uint64_t)initrd->address, "zap-light24.psf");
    init_text_renderer(framebuffer, font);
    
    puts("Starstone 1.0 Ahlspiess\n");
    printf("%%d: %d\n%%s: %s\n%%x: %x\n%%c: %c\n", 123, "Hello world!", 0x12ab, '!');
    
    while(1);
    
}
