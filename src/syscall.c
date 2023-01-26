#include "common.h"
#include "terminal.h"

void sys_write(uint64_t fd, void* buf, uint64_t size) {
	
	
	if(fd == 0) { // STDOUT
		
		for(uint64_t i = 0; i < size; i++) terminal->putc(terminal, ((char*)buf)[i]);
		
	}
	
}

void syscall(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
	
	UNUSED(arg3);
	UNUSED(arg4);
	UNUSED(arg5);
	if(num == 1) sys_write(arg0, (void*)arg1, arg2);
	
}