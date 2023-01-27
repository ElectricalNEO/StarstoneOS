[bits 64]
[org 0x1000]

mov rax, 1
mov rdi, 0
mov rsi, helloworld
mov rdx, helloworld_len
syscall
jmp $

helloworld: db "Hello World!", 10
helloworld_len equ $ - helloworld
