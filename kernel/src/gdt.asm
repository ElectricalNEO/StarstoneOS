[bits 64]

global gdtr
global gdtr.addr
global gdt.tss_base_0
global gdt.tss_base_1
global gdt.tss_base_2
global gdt.tss_base_3

section .boot.data

gdtr:
.size: dw gdt.end - gdt - 1
.addr: dq gdt

gdt:
.null:
	dd 0
	dd 0
.code_kernel:
	dd 0xffff
	dd (10 << 8) | (1 << 12) | (1 << 15) | (0xf << 16) | (1 << 21) | (1 << 23)
.data_kernel:
	dd 0xffff
	dd (2 << 8) | (1 << 12) | (1 << 15) | (0xf << 16) | (1 << 21) | (1 << 23)
.data_user:
	dd 0xffff
	dd (2 << 8) | (1 << 12) | (3 << 13) | (1 << 15) | (0xf << 16) | (1 << 21) | (1 << 23)
.code_user:
	dd 0xffff
	dd (10 << 8) | (1 << 12) | (3 << 13) | (1 << 15) | (0xf << 16) | (1 << 21) | (1 << 23)
.tss:
	dw 104
.tss_base_0: dw 0
.tss_base_1: db 0
	db 0x89
	db 0x40
.tss_base_2: db 0
.tss_base_3: dd 0
	dd 0
.end: