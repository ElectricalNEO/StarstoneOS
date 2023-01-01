ENTRY(_start)

SECTIONS {
	
	. = 0x100000;

	.bootstrap.text : ALIGN (0x1000) {
		*(.multiboot)
		*(.bootstrap.text)
	}
	
	.bootstrap.data : ALIGN (0x1000) {
		*(.bootstrap.data)
	}
	
	.bootstrap.bss : ALIGN (0x1000) {
		*(.bootstrap.bss)
	}
	
	. += 0xffffffff80000000;
	
	.text ALIGN (0x1000) : AT (ADDR(.text) - 0xffffffff80000000) {
		*(.text)
	}
	
	.data ALIGN (0x1000) : AT (ADDR(.data) - 0xffffffff80000000) {
		*(.data)
	}
	
	.bss : ALIGN (0x1000) {
		*(COMMON)
		*(.bss)
	}
	
	.rodata ALIGN (0x1000) : AT (ADDR(.rodata) - 0xffffffff80000000) {
		*(.rodata)
		*(.rodata.*)
	}
	
}
