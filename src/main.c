#include "common.h"
#include "framebuffer.h"
#include "initrd.h"
#include "tar.h"
#include "psf.h"
#include "text_renderer.h"

int main(struct framebuffer* framebuffer, struct initrd* initrd) {
    
    struct psf_header* font = tar_open_file((void*)(uint64_t)initrd->address, "zap-light24.psf");
    init_text_renderer(framebuffer, font);
    
    puts("Hello world!");
    
    while(1);
    
}
