[bits 64]

MULTIBOOT_MAGIC equ 0xe85250d6
MULTIBOOT_ARCHITECTURE equ 0

section .multiboot

align 8
multiboot_header:
dd MULTIBOOT_MAGIC
dd MULTIBOOT_ARCHITECTURE
dd .end - multiboot_header
dd -(MULTIBOOT_MAGIC + MULTIBOOT_ARCHITECTURE + .end - multiboot_header)

align 8
.framebuffer_tag:
dw 5	; TYPE
dw 0	; FLAGS
dd 20	; SIZE
dd 0	; WIDTH
dd 0	; HEIGHT
dd 32	; DEPTH

align 8
.align_tag:
dw 6	; TYPE
dw 0	; FLAGS
dd 8	; SIZE

align 8
.end_tag:
dw 0	; TYPE
dw 0	; FLAGS
dd 8	; SIZE

.end: