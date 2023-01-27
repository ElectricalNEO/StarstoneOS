[bits 64]

extern switch_task
extern tick
extern syscall
extern lock_task
extern unlock_task

global task_registers
global int_20h
global int_80h

section .text

int_20h:
	
	push rax
	push rbx
	push rcx
	push rdx
	
	push rsi
	push rdi
	push rbp
	
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	mov rax, cr3
	push rax
	
	call tick
	
	mov [task_registers], rsp
	call switch_task
	
	pop rax
	mov cr3, rax
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	
	pop rbp
	pop rdi
	pop rsi
	
	pop rdx
	pop rcx
	pop rbx
	
	mov al, 0x20
	out 0x20, al
	
	pop rax
	
	iretq

int_80h:
	push r9
	push rcx
	push rdx
	push rsi
	push rdi
	push r8
	push rax
	push r10
	push r11
	
	mov r9, r8
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, rax
	mov r8, r10
	
	call lock_task
	call syscall
	call unlock_task
	
	pop r11
	pop r10
	pop rax
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop r9
	iretq

int_81h:
	
	push rax
	push rbx
	push rcx
	push rdx
	
	push rsi
	push rdi
	push rbp
	
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	mov rax, cr3
	push rax
	
	mov [task_registers], rsp
	call switch_task
	
	pop rax
	mov cr3, rax
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	
	pop rbp
	pop rdi
	pop rsi
	
	pop rdx
	pop rcx
	pop rbx
	pop rax
	
	iretq

section .bss
task_registers: resq 1
