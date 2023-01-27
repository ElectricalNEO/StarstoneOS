#pragma once
#include "../common.h"
#include "../list.h"

struct registers {
	
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
	
};

struct task {
	
	struct registers regs;
	char name[64];
	
};

LINKED_LIST(struct task*, task_list_node);

extern struct task_list_node task_list;

uint8_t start_task(void(*entry_point)(), char* name, uint64_t page_table);
void switch_task();

void lock_task();
void unlock_task();
