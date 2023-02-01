#pragma once
#include <common.hpp>


namespace syscalls {
	
	extern "C" void init_syscall();
	
	void syscall(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5);
	
	void sys_write(uint64_t fd, uint64_t buf, uint64_t count);
	
}
