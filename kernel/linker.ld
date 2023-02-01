ENTRY(_start)

SECTIONS {
	
	. = 0x100000;
	
	.boot.text : ALIGN(4K) {
		*(.multiboot)
		*(.boot.text)
	}
	
	.boot.data : ALIGN(4K) {
		*(.boot.data)
	}
	
	.boot.bss : ALIGN(4K) {
		*(.boot.bss)
	}
	
	. += 0xffffff0000000000;
	
	.text ALIGN(4K) : AT (ADDR(.text) - 0xffffff0000000000) {
		*(.text*)
	}
	
	.data ALIGN(4K) : AT (ADDR(.data) - 0xffffff0000000000) {
		start_ctors = .;
		KEEP(*(.init_array));
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)));
		end_ctors = .;
		
		*(.data)
	}
	
	.rodata ALIGN(4K) : AT (ADDR(.rodata) - 0xffffff0000000000) {
		*(.rodata*)
	}
	
	.bss ALIGN(4K) : AT (ADDR(.bss) - 0xffffff0000000000) {
		*(.bss)
	}
	
	/DISCARD/ : { *.(fini_array*) *(.comment) }
	
	KERNEL_END = ALIGN(4K);
	
}
