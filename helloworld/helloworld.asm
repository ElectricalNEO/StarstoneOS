[bits 64]

global _start

section .text

_start:
mov rax, 1
mov rdi, 1
mov rsi, helloworld
mov rdx, HELLOWORLD_LEN

syscall

jmp $

helloworld: db "Hello world!", 10
HELLOWORLD_LEN equ $ - helloworld
