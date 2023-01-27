[bits 64]

global init_syscall

section .text

_syscall:
	push rcx
	int 80h
	pop rcx
	o64 sysret

init_syscall:
	mov rcx, 0xc0000080
	rdmsr
	or rax, 1
	wrmsr
	
	mov rdx, 0x8 | (((8 * 2) | 3) << 16)
	xor rax, rax
	mov rcx, 0xc0000081
	wrmsr
	
	mov rdx, _syscall
	shr rdx, 32
	mov rax, _syscall
	mov rcx, 0xc0000082
	wrmsr
	
	xor rdx, rdx
	xor rax, rax
	mov rcx, 0xc0000084
	wrmsr
	
	retq
