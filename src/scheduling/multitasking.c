#include "multitasking.h"
#include "../memory/memory.h"
#include "../memory/heap.h"
#include "../terminal.h"

struct task_list_node task_list = {
	
	.data = 0,
	.next = 0
	
};

struct task_list_node* current_task = 0;
extern struct registers* task_registers;

uint8_t start_task(void(*entry_point)(), char* name, uint64_t page_table) {
	
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
	
	struct task_list_node* node = &task_list;
	while(node->data && node->next) {
		
		node = node->next;
		
	}
	
	if(node->data) {
		
		node->next = malloc(sizeof(struct task_list_node));
		if(!node->next) {
			
			free((void*)task->regs.rsp);
			free(task);
			return 1;
			
		}
		node = node->next;
		node->next = 0;
		
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
	
	if(current_task) {
		
		memcpy(task_registers, &current_task->data->regs, sizeof(struct registers));
		current_task = current_task->next;
		
	}
	if(!current_task) current_task = &task_list;
	
	memcpy(&current_task->data->regs, task_registers, sizeof(struct registers));
	
}
