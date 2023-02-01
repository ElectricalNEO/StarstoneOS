#include <string.hpp>
#include <memory/Heap.hpp>
#include <TextRenderer.hpp>
#include <memory/memory.hpp>
#include <memory/PageTable.hpp>
#include <multitasking/Scheduler.hpp>

using namespace multitasking;

Scheduler::TaskListNode* Scheduler::taskList;
Scheduler::TaskListNode* Scheduler::curTask;
Scheduler::Registers* Scheduler::taskRegisters = 0;
uint8_t Scheduler::locked = 0, Scheduler::switchWaiting = 0;

uint8_t Scheduler::startTask(uint64_t entry, uint64_t stack, uint64_t cs, uint64_t ds, memory::PageTable* pageTable) {
	
	TaskListNode* node;
	
	if(!(uint64_t)taskList) {
		
		taskList = (TaskListNode*)memory::Heap::malloc(sizeof(TaskListNode));
		if(!(uint64_t)taskList) return 1;
		taskList->next = taskList;
		node = taskList;
		curTask = taskList;
		
	} else {
		
		node = (TaskListNode*)memory::Heap::malloc(sizeof(TaskListNode));
		if(!(uint64_t)node) return 1;
		node->next = taskList->next;
		taskList->next = node;
		
	}
	
	node->task.regs.cr3 = pageTable->pml4;
	node->task.regs.rip = entry;
	node->task.regs.cs = cs;
	node->task.regs.rflags = 0x202;
	node->task.regs.rsp = stack;
	node->task.regs.ss = ds;
	
	return 0;
	
}

void Scheduler::switchTask() {
	
	if(locked) {
		
		switchWaiting = 1;
		return;
		
	}
	
	memory::memcpy(taskRegisters, &curTask->task.regs, sizeof(Registers));
	curTask = curTask->next;
	memory::memcpy(&curTask->task.regs, taskRegisters, sizeof(Registers));
	
}

void Scheduler::lock() {
	
	locked = 1;
	
}

void Scheduler::unlock() {
	
	locked = 0;
	if(switchWaiting) {
		
		switchWaiting = 0;
		asm("int 0x81");
		
	}
	
}
