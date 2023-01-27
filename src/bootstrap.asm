[bits 32]

KERNEL_VIRT equ 0xffffffff80000000
FRAMEBUFFER_VIRT equ 0xffffff8040000000
INITRD_VIRT equ 0xffffff8080000000

extern main
global _start
global pml4
global pdpt
global tss
global stack

MAGIC equ 0xe85250d6
ARCHITECTURE equ 0 ; i386

ERROR equ 0x31212012
ERR_NO_CPUID equ 0x31212000
ERR_NO_CPUID_EXTENDED equ 0x31212001
ERR_NO_LONG_MODE equ 0x31212002
ERR_NO_FRAMEBUFFER equ 0x31212003
ERR_NO_INITRD equ 0x31212004

section .multiboot

align 8
header:
dd MAGIC
dd ARCHITECTURE
dd .end - header ; LENGTH
dd -(MAGIC + ARCHITECTURE + (.end - header)) ; CHECKSUM

align 8
.framebuffer_tag:
dw 5 ; TYPE
dw 0 ; FLAGS
dd 20 ; SIZE
dd 0 ; WIDTH
dd 0 ; HEIGHT
dd 32 ; DEPTH

align 8
.align_tag:
dw 6 ; TYPE
dw 0 ; FLAGS
dd 8 ; SIZE

align 8
.end_tag:
dw 0 ; TYPE
dw 0 ; FLAGS
dd 8 ; SIZE
.end:

section .text

_start:
	;;;;;;;;;;;;;;;;;;;
	;;; SETUP STACK ;;;
	;;;;;;;;;;;;;;;;;;;
	mov esp, stack
	push ebx
	
	;;;;;;;;;;;;;;;;;;;;;;;
	;;; CHECK FOR CPUID ;;;
	;;;;;;;;;;;;;;;;;;;;;;;
	pushfd
	pop eax
	mov ecx, eax
	xor eax, 1 << 21
	push eax
	popfd
	pushfd
	pop eax
	push ecx
	popfd
	xor eax, ecx
	mov eax, ERR_NO_CPUID
	jz error
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;; CHECK FOR EXTENDED FUNCTIONS OF CPUID ;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	mov eax, ERR_NO_CPUID_EXTENDED
	jb error
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;; CHECK FOR LONG MODE ;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29
	mov eax, ERR_NO_LONG_MODE
	jz error
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;; PARSE MULTIBOOT2 TAGS  ;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	pop ebx
	add ebx, 8
	
.parse_tags:
	cmp dword [ebx], 0
	je .end_parse_tags
	
	cmp dword [ebx], 8
	jne .skip_framebuffer_tag
	lea esi, [ebx + 8]
	mov edi, framebuffer
	mov ecx, 21
	rep movsb
	
.skip_framebuffer_tag:
	cmp dword [ebx], 3
	jne .skip_module_tag
	
	lea esi, [ebx + 16]
	lea edi, [initrd_name]
	mov ecx, [initrd_name_len]
	repe cmpsb
	
	jne .skip_module_tag
	
	mov esi, [ebx + 8]
	mov [initrd_grub.address], esi
	mov esi, [ebx + 12]
	sub esi, [initrd_grub.address]
	mov [initrd_grub.size], esi
	
.skip_module_tag:
	cmp dword [ebx], 6
	jne .skip_memory_map_tag
	
	sub esp, [ebx + 4]
	add esp, 4
	mov [memory_map], esp
	
	lea esi, [ebx + 4]
	mov edi, [memory_map]
	mov ecx, [ebx + 4]
	sub ecx, 4
	rep movsb
	
.skip_memory_map_tag:
	mov edx, [ebx + 4]
	add edx, 7
	and edx, 0b11111111111111111111111111111000
	add ebx, edx
	jmp .parse_tags
	
.end_parse_tags:
	cmp dword [framebuffer.width], 0
	mov eax, ERR_NO_FRAMEBUFFER
	je error
	
	cmp dword [initrd_grub.address], 0
	mov eax, ERR_NO_INITRD
	je error
	
	;;;;;;;;;;;;;;;;;;;;
	;;; CLEAR TABLES ;;;
	;;;;;;;;;;;;;;;;;;;;
	xor ecx, ecx
	
.loop:
	lea eax, [pml4 + ecx * 4]
	lea eax, [pdpt + ecx * 4]
	lea eax, [pd_kernel + ecx * 4]
	lea eax, [pd_framebuffer + ecx * 4]
	lea eax, [pd_initrd + ecx * 4]
	mov dword [eax], 0
	inc ecx
	cmp ecx, 1024
	jne .loop
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;; MAP KERNEL & ACTIVATE RECURSIVE MAPPING ;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	mov eax, pdpt
	or eax, 0b111
	mov [pml4], eax
	mov [pml4 + 511 * 8], eax
	
	mov eax, pd_kernel
	or eax, 0b111
	mov [pdpt], eax
	mov [pdpt + 510 * 8], eax
	
	mov eax, 0b10000111
	mov [pd_kernel], eax
	
	mov eax, pml4
	or eax, 0b111
	mov [pml4 + 510 * 8], eax
	
	;;;;;;;;;;;;;;;;;;
	;;; ENABLE PAE ;;;
	;;;;;;;;;;;;;;;;;;
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax
	
	;;;;;;;;;;;;;;;
	;;; SET LME ;;;
	;;;;;;;;;;;;;;;
	mov ecx, 0xc0000080
	rdmsr
	or eax, 1 << 8
	wrmsr
	
	;;;;;;;;;;;;;;;;;;;;;
	;;; ENABLE PAGING ;;;
	;;;;;;;;;;;;;;;;;;;;;
	mov eax, pml4
	mov cr3, eax
	
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax
	
	;;;;;;;;;;;;;;;;;;;;;;;;;
	;;; LOAD GDT AND JUMP ;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;
	lgdt [gdtr_bootstrap]
	jmp dword 8:start64

error:
	mov ebx, ERROR
	mov ecx, ERROR
	mov edx, ERROR
	jmp $

section .bootstrap.data

gdtr_bootstrap:
	dw gdt_bootstrap.end - gdt_bootstrap - 1
	dq gdt_bootstrap

gdt_bootstrap:
.null:
	dd 0
	dd 0
.code:
	dd 0xffff
	dd (10 << 8) | (1 << 12) | (1 << 15) | (0xf << 16) | (1 << 21) | (1 << 23)
.data:
	dd 0xffff
	dd (2 << 8) | (1 << 12) | (1 << 15) | (0xf << 16) | (1 << 21) | (1 << 23)
.end:

initrd_name: db "initrd", 0
initrd_name_len: db $ - initrd_name

section .bootstrap.bss nobits

align 4096
pml4: resb 4096

align 4096
pdpt: resb 4096

align 4096
pd_kernel: resb 4096

align 4096
pd_framebuffer: resb 4096

align 4096
pd_initrd: resb 4096

resb 16*1024
stack: ; stack grows downwards

framebuffer:
.address: resq 1
.pitch: resd 1
.width: resd 1
.height: resd 1
.bpp: resb 1

initrd_grub:
.address: resd 1
.size: resd 1

initrd:
.address: resq 1
.size: resd 1

memory_map: resd 1

tss: resb 104

[bits 64]
section .bootstrap.text

start64:
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;; FIX STACK AND LOAD NEW GDT ;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	add rsp, KERNEL_VIRT
	lgdt [gdtr]
	
	;;;;;;;;;;;;;;;;;;;;;;;
	;;; RELOAD SEGMENTS ;;;
	;;;;;;;;;;;;;;;;;;;;;;;
	push 8
	lea rax, [rel .reload_cs]
	push rax
	retfq
.reload_cs:
	mov rax, 0x10
	mov ds, rax
	mov es, rax
	mov fs, rax
	mov gs, rax
	mov ss, rax
	
	;;;;;;;;;;;;;;;;;
	;;; SETUP TSS ;;;
	;;;;;;;;;;;;;;;;;
	mov rax, tss
	add rax, KERNEL_VIRT
	mov [gdt.tss_base_0], ax
	shr rax, 16
	mov [gdt.tss_base_1], al
	mov [gdt.tss_base_2], ah
	shr rax, 16
	mov [gdt.tss_base_3], eax
	
	;;;;;;;;;;;;;;;;;;;;;;;
	;;; MAP FRAMEBUFFER ;;;
	;;;;;;;;;;;;;;;;;;;;;;;
	mov eax, pd_framebuffer
	or eax, 0b111
	mov [pdpt + 8], eax
	
	mov eax, [framebuffer.pitch]
	mul dword [framebuffer.height]
	add eax, 0x200000 - 1
	xor edx, edx
	mov ecx, 0x200000
	div dword ecx
	mov edx, eax
	
	mov rax, [framebuffer.address]
	and rax, 0xffffffffffe00000
	mov rbx, 1 << 48
	test rax, rbx
	jz .skip_fb_c
	mov rbx, 0xffff000000000000
	or rax, rbx
.skip_fb_c:
	xor ecx, ecx
.map_fb_loop:
	mov rbx, rax
	or rbx, (1 << 0) | (1 << 1) | (1 << 7)
	mov [pd_framebuffer + ecx * 8], rbx
	inc ecx
	add rax, 0x200000
	cmp ecx, edx
	jne .map_fb_loop
	
	mov rax, [framebuffer.address]
	and rax, 0x1fffff
	mov rbx, FRAMEBUFFER_VIRT
	add rax, rbx
	mov [framebuffer.address], rax
	
	;;;;;;;;;;;;;;;;;;
	;;; MAP INITRD ;;;
	;;;;;;;;;;;;;;;;;;
	mov eax, pd_initrd
	or eax, 0b111
	mov [pdpt + 8 * 2], eax
	
	mov eax, [initrd_grub.size]
	add eax, 0x200000 - 1
	xor edx, edx
	mov ecx, 0x200000
	div dword ecx
	mov edx, eax
	
	xor rax, rax
	mov eax, [initrd_grub.address]
	and rax, 0xffffffffffe00000
	mov rbx, 1 << 48
	test rax, rbx
	jz .skip_initrd_c
	mov rbx, 0xffff000000000000
	or rax, rbx
.skip_initrd_c:
	xor ecx, ecx
.map_initrd_loop:
	mov rbx, rax
	or rbx, (1 << 0) | (1 << 1) | (1 << 7)
	mov [pd_initrd + ecx * 8], rbx
	inc ecx
	add rax, 0x200000
	cmp ecx, edx
	jne .map_initrd_loop
	
	xor rax, rax
	mov eax, [initrd_grub.address]
	and rax, 0x1fffff
	mov rbx, INITRD_VIRT
	add rax, rbx
	mov [initrd.address], rax
	mov eax, [initrd_grub.size]
	mov [initrd.size], eax
	
	;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;; JUMP TO HIGHER HALF ;;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;
	mov rax, 0xffffffff00000000
	or rax, higher_half
	jmp rax

section .text
higher_half:
	mov qword [pml4], 0
	;;;;;;;;;;;;;;;;;
	;;; CALL MAIN ;;;
	;;;;;;;;;;;;;;;;;
	lea rdi, [framebuffer + KERNEL_VIRT]
	lea rsi, [initrd + KERNEL_VIRT]
	xor rdx, rdx
	mov edx, [memory_map]
	add rdx, KERNEL_VIRT
	
	mov rax, main
	call rax
	jmp $

section .data

gdtr:
	dw gdt.end - gdt - 1
	dq gdt

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
