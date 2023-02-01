[bits 64]

KERNEL_VIRT equ 0xffffff0000000000

extern _ZN12multitasking9Scheduler10switchTaskEv
extern _ZN12multitasking9Scheduler13taskRegistersE
extern _Z7syscallyyyyyyy
extern tss
global int_20
global int_80
global int_81

section .text

int_20:
	
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
	
	mov rax, tss
	add rax, 4
	mov [rax], rsp
	
	mov rax, _ZN12multitasking9Scheduler13taskRegistersE
	mov [rax], rsp
	call _ZN12multitasking9Scheduler10switchTaskEv
	
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

int_80:
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
	
	call _Z7syscallyyyyyyy
	
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

int_81:
	
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
	
	mov rax, tss
	add rax, 4
	mov [rax], rsp
	
	mov rax, _ZN12multitasking9Scheduler13taskRegistersE
	mov [rax], rsp
	call _ZN12multitasking9Scheduler10switchTaskEv
	
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
