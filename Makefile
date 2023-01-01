
# https://stackoverflow.com/a/18258352
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SOURCES = $(call rwildcard, src, *.asm *.c)
OBJECTS_ASM = $(patsubst src/%.asm, bin/%_asm.o, $(SOURCES))
OBJECTS = $(patsubst src/%.c, bin/%_c.o, $(OBJECTS_ASM))

default: Starstone.iso

bin/%_asm.o: src/%.asm
	mkdir -p $(@D)
	nasm -f elf64 $< -o $@

bin/%_c.o: src/%.c
	mkdir -p $(@D)
	gcc -m64 -c $< -o $@ -ffreestanding -z max-page-size=0x1000 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fno-asynchronous-unwind-tables -std=gnu99 -O2 -Wall -Wextra -masm=intel

bin/starkrnl: linker.ld $(OBJECTS)
	ld -o $@ -T $^

initrd.tar: initrd
	cd $< && tar -cf ../$@ *

Starstone.iso: bin/starkrnl grub.cfg initrd.tar
	mkdir -p iso/boot/grub
	cp bin/starkrnl iso/boot/
	cp grub.cfg iso/boot/grub/
	cp initrd.tar iso/boot/
	grub-mkrescue -o $@ iso
	rm -r iso
	
run: Starstone.iso
	qemu-system-x86_64 -cdrom $<

clean:
	-rm -r bin
	-rm Starstone.iso
	-rm initrd.tar