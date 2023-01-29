#include "multitasking.h"
#include "../memory/memory.h"
#include "../memory/heap.h"
#include "../terminal.h"

struct task_list_node task_list = {
	
	.data = 0,
	.next = &task_list,
	.prev = &task_list
	
};

struct task_list_node* current_task = &task_list;
extern struct registers* task_registers;
uint8_t lock = 0, switch_waiting = 0;

uint8_t start_task(void(*entry_point)(), char* name, uint64_t page_table, uint64_t code_segment, uint64_t data_segment) {
	
	struct task* task = malloc(sizeof(struct task));
	if(!task) return 1;
	
	memset(&task->regs, 0, sizeof(struct registers));
	task->regs.rsp = (uint64_t)malloc(4096);
	if(!task->regs.rsp) {
		
		free(task);
		return 1;
		
	}
	task->regs.rsp += 4096;
	task->regs.rip = (uint64_t)entry_point;
	task->regs.rflags = 0x202;
	task->regs.cs = 0x8;
	task->regs.ss = 0x10;
	task->regs.cr3 = page_table;
	task->regs.ss = data_segment;
	task->regs.cs = code_segment;
	
	struct task_list_node* node = &task_list;
	if(node->data) {
		
		struct task_list_node* next = node->next;
		struct task_list_node* new = malloc(sizeof(struct task_list_node));
		if(!new) {
			
			free((void*)(task->regs.rsp - 4096));
			free(task);
			return 1;
			
		}
		node->next = new;
		node->next->next = next;
		node->next->prev = node;
		node = node->next;
		
	}
	
	uint8_t i = 0;
	for(; i < 63 && name[i]; i++) {
		
		task->name[i] = name[i];
		
	}
	task->name[i] = 0;
	
	node->data = task;
	
	return 0;
	
}

void switch_task() {
	
	if(lock) {
		
		switch_waiting = 1;
		return;
		
	}
	
	memcpy(task_registers, &current_task->data->regs, sizeof(struct registers));
	current_task = current_task->next;
	
	memcpy(&current_task->data->regs, task_registers, sizeof(struct registers));
	
}

void lock_task() {
	
	lock = 1;
	
}

void unlock_task() {
	
	lock = 0;
	if(switch_waiting) asm("int 0x81");
	
}

void terminate_current_task() {
	
	lock = 1;
	
	current_task->next->prev = current_task->prev;
	current_task->prev->next = current_task->next;
	
	free(current_task->data);
	free(current_task);
	
	lock = 0;
	switch_waiting = 0;
	asm("int 0x81");
	
}
