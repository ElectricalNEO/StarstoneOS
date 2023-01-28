[bits 64]

global _start

section .text

_start:
	
	mov rax, 1
	mov rdi, 0
	mov rsi, helloworld
	mov rdx, helloworld_len
	syscall
	
	mov rdx, 20
	mov rsi, test
	
	mov cx, 0
	mov bl, 'a'
.loop:
	mov byte [rsi], bl
	inc rsi
	inc cx
	inc bl
	cmp cx, 20
	jne .loop
	
	mov rax, 1
	mov rdi, 0
	mov rsi, test
	mov rdx, 20
	syscall
	
	jmp $

section .data

helloworld: db "Hello World!", 10
helloworld_len equ $ - helloworld

section .bss
test: resb 20
