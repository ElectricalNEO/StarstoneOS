#include <TextRenderer.hpp>
#include <syscalls/syscalls.hpp>

using namespace syscalls;

void syscalls::sys_write(uint64_t fd, uint64_t buf, uint64_t count) {
	
	if(fd == 1) { // STDOUT
		
		for(uint64_t i = 0; i < count; i++) TextRenderer::putc(*(uint8_t*)(buf + i));
		
	}
	
}
