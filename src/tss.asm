[bits 64]

global flush_tss

flush_tss:
	push rax
	mov ax, 5 * 8
	ltr ax
	pop rax
	retq
