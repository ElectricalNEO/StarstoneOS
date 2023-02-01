[bits 32]

KERNEL_VIRT equ 0xffffff0000000000
FRAMEBUFFER_VIRT equ 0xffffff0040000000
INITRD_VIRT equ 0xffffff0080000000

global _start
global pml4
global tss
extern kmain
extern gdtr
extern gdtr.addr
extern gdt.tss_base_0
extern gdt.tss_base_1
extern gdt.tss_base_2
extern gdt.tss_base_3

section .boot.text

_start:
	mov esp, stack
	add ebx, 8
	
	call parse_tags
	call cpuid_check
	call cpuid_extended_check
	call long_mode_check
	call clear_tables
	call map_kernel
	call enable_pae
	call set_lme
	call enable_paging
	
	lgdt [gdtr]
	jmp dword 8:start64

parse_tags:
	cmp dword [ebx], 0
	je .end
	
	cmp dword [ebx], 3
	je .module
	
	cmp dword [ebx], 6
	je .memory_map
	
	cmp dword [ebx], 8
	je .framebuffer
	
	jmp .skip
	
.module:
	lea esi, [ebx + 16]
	lea edi, [initrd_name]
	mov ecx, INITRD_NAME_LEN
	repe cmpsb
	
	jne .skip
	
	mov esi, [ebx + 8]
	mov [initrd.address], esi
	mov dword [initrd.address + 4], 0
	
	mov esi, [ebx + 12]
	sub esi, [initrd.address + 4]
	mov [initrd.size], esi
	
	jmp .skip
	
.memory_map:
	pop eax
	sub esp, [ebx + 4]
	add esp, 4
	mov [memory_map], esp
	mov dword [memory_map + 4], 0
	
	lea esi, [ebx + 4]
	mov edi, [memory_map]
	mov ecx, [ebx + 4]
	sub ecx, 4
	rep movsb
	push eax
	
	jmp .skip
	
.framebuffer:
	lea esi, [ebx + 8]
	mov edi, framebuffer
	mov ecx, 21
	rep movsb
	
.skip:
	mov edx, [ebx + 4]
	add edx, 7
	and edx, ~7
	add ebx, edx
	jmp parse_tags
	
.end:
	cmp dword [framebuffer.width], 0
	je .error
	
	cmp dword [initrd.size], 0
	je .error
	
	cmp dword [memory_map], 0
	je .error
	
	ret
	
.error:
	jmp $

cpuid_check:
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
	
	jz .error
	ret
.error:
	jmp $

cpuid_extended_check:
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	
	jb .error
	ret
.error:
	jmp $

long_mode_check:
	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29
	
	jz .error
	ret
.error:
	jmp $

clear_tables:
	xor ecx, ecx
.loop:
	mov dword [pml4 + ecx * 4], 0
	mov dword [pdpt + ecx * 4], 0
	mov dword [pd_kernel + ecx * 4], 0
	mov dword [pd_framebuffer + ecx * 4], 0
	mov dword [pd_initrd + ecx * 4], 0
	inc ecx
	cmp ecx, 1024
	jne .loop
	ret

map_kernel:
	mov eax, pdpt
	or eax, 0b111
	mov [pml4], eax
	mov [pml4 + 510 * 8], eax
	
	mov eax, pd_kernel
	or eax, 0b111
	mov [pdpt], eax
	
	mov dword [pd_kernel], 0b10000011
	
	mov eax, pml4
	or eax, 0b11
	mov dword [pml4 + 511 * 8], eax
	
	ret

enable_pae:
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax
	ret

set_lme:
	mov ecx, 0xc0000080
	rdmsr
	or eax, 1 << 8
	wrmsr
	ret

enable_paging:
	mov eax, pml4
	mov cr3, eax
	
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax
	ret

section .boot.data

initrd_name: db "initrd", 0
INITRD_NAME_LEN equ $ - initrd_name

section .boot.bss nobits

resb 16*1024
stack:

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

framebuffer:
.address: resq 1
.pitch: resd 1
.width: resd 1
.height: resd 1
.bpp: resb 1

initrd:
.address: resq 1
.size: resd 1

memory_map: resq 1

[bits 64]

section .boot.text

start64:
	mov rax, higher_half
	jmp rax

section .text

higher_half:
	mov rax, KERNEL_VIRT
	add rsp, rax
	add qword [gdtr.addr], rax
	call setup_tss
	lgdt [gdtr]
	
	push 8
	lea rax, [rel .reload]
	push rax
	retfq
.reload:
	mov rax, 0x10
	mov ds, rax
	mov es, rax
	mov fs, rax
	mov gs, rax
	mov ss, rax
	
	call map_framebuffer
	call map_initrd
	
	mov rdi, framebuffer + KERNEL_VIRT
	mov rsi, initrd + KERNEL_VIRT
	mov rdx, [memory_map]
	mov rax, KERNEL_VIRT
	add rdx, rax
	
	mov qword [pml4], 0
	invlpg [0]
	call kmain
	
	jmp $

setup_tss:
	mov rax, tss
	mov rbx, KERNEL_VIRT
	
	mov word [rax + 0x66], 104
	
	mov [gdt.tss_base_0], ax
	shr rax, 16
	mov [gdt.tss_base_1], al
	mov [gdt.tss_base_2], ah
	shr rax, 16
	mov [gdt.tss_base_3], eax
	
	mov ax, 0x28
	ltr ax
	
	ret

map_framebuffer:
	mov eax, pd_framebuffer
	or eax, 0b11
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
	xor ecx, ecx
.loop:
	mov rbx, rax
	or rbx, 0b10000011
	mov [pd_framebuffer + ecx * 8], rbx
	inc ecx
	add rax, 0x200000
	cmp ecx, edx
	jne .loop
	
	mov rax, [framebuffer.address]
	and rax, 0x1fffff
	mov rbx, FRAMEBUFFER_VIRT
	add rax, rbx
	mov [framebuffer.address], rax
	
	ret

map_initrd:
	mov eax, pd_initrd
	or eax, 0b11
	mov [pdpt + 16], eax
	
	mov eax, [initrd.size]
	add eax, 0x200000 - 1
	xor edx, edx
	mov ecx, 0x200000
	div dword ecx
	mov edx, eax
	
	mov rax, [initrd.address]
	and rax, 0xffffffffffe00000
	xor ecx, ecx
.loop:
	mov rbx, rax
	or rbx, 0b10000011
	mov [pd_initrd + ecx * 8], rbx
	inc ecx
	add rax, 0x200000
	cmp ecx, edx
	jne .loop
	
	mov rax, [initrd.address]
	and rax, 0x1fffff
	mov rbx, INITRD_VIRT
	add rax, rbx
	mov [initrd.address], rax
	
	ret

section .bss
tss: resq 13
