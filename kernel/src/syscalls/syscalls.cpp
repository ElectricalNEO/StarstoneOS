#include <syscalls/syscalls.hpp>
#include <multitasking/Scheduler.hpp>

using namespace syscalls;

void syscall(uint64_t num, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5) {
	
	multitasking::Scheduler::lock();
	if(num == 1) sys_write(arg0, arg1, arg2);
	multitasking::Scheduler::unlock();
	
}
