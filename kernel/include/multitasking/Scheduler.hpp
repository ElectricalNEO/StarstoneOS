#pragma once
#include <common.hpp>
#include <memory/PageTable.hpp>

namespace multitasking {
	
	class Scheduler {
		
		struct Registers {
			
			uint64_t cr3;
			uint64_t r15;
			uint64_t r14;
			uint64_t r13;
			uint64_t r12;
			uint64_t r11;
			uint64_t r10;
			uint64_t r9;
			uint64_t r8;
			
			uint64_t rbp;
			uint64_t rdi;
			uint64_t rsi;
			
			uint64_t rdx;
			uint64_t rcx;
			uint64_t rbx;
			uint64_t rax;
			
			uint64_t rip;
			uint64_t cs;
			uint64_t rflags;
			uint64_t rsp;
			uint64_t ss;
		
		}__attribute__((packed));
		
		struct Task {
			
			Registers regs;
			
		};
		
		struct TaskListNode {
			
			TaskListNode* next;
			Task task;
			
		};
		
		static Registers* taskRegisters;
	 	static TaskListNode* taskList;
		static TaskListNode* curTask;
		static uint8_t locked, switchWaiting;
		
	public:
		static uint8_t startTask(uint64_t entry, uint64_t stack, uint64_t cs, uint64_t ds, memory::PageTable* pageTable);
		static void switchTask();
		static void lock();
		static void unlock();
		
	};
	
}
