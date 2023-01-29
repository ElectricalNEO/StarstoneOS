
# https://stackoverflow.com/a/18258352
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ROOT = .

SOURCES = $(call rwildcard, src, *.asm *.c)
OBJECTS_ASM = $(patsubst src/%.asm, bin/%_asm.o, $(SOURCES))
OBJECTS = $(patsubst src/%.c, bin/%_c.o, $(OBJECTS_ASM))

default: Starstone.iso

bin/%_asm.o: src/%.asm
	@echo "\033[36;1mCOMPILING $@...\033[0m"
	@mkdir -p $(@D)
	@nasm -f elf64 $< -o $@

bin/%_c.o: src/%.c
	@echo "\033[36;1mCOMPILING $@...\033[0m"
	@mkdir -p $(@D)
	@gcc -m64 -c $< -o $@ -ffreestanding -z max-page-size=0x1000 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fno-asynchronous-unwind-tables -std=gnu99 -O0 -Wall -Wextra -masm=intel

bin/interrupts/interrupts_c.o: src/interrupts/interrupts.c
	@echo "\033[36;1mCOMPILING $@...\033[0m"
	@mkdir -p $(@D)
	@gcc -m64 -c $< -o $@ -ffreestanding -z max-page-size=0x1000 -mno-red-zone -mgeneral-regs-only -mno-mmx -mno-sse -mno-sse2 -fno-asynchronous-unwind-tables -std=gnu99 -O0 -Wall -Wextra -masm=intel

bin/starkrnl: linker.ld $(OBJECTS)
	@echo "\033[35;1mLINKING $@...\033[0m"
	@ld -o $@ -T $^

initrd.tar: initrd bin/apps/app
	@echo "\033[33;1mGENERATING $@...\033[0m"
	@cp bin/apps/app initrd/
	@cd $< && tar -cf ../$@ *

Starstone.iso: bin/starkrnl grub.cfg initrd.tar
	@echo "\033[33;1mGENERATING $@...\033[0m"
	@mkdir -p iso/boot/grub
	@cp bin/starkrnl iso/boot/
	@cp grub.cfg iso/boot/grub/
	@cp initrd.tar iso/boot/
	@grub-mkrescue -o $@ iso 2> /dev/null
	@rm -r iso
	
run: Starstone.iso
	@echo "\033[32;1mRUNNING...\033[0m"
	@qemu-system-x86_64 -cdrom $< -m 1G

clean:
	@echo "\033[31;1mCLEANING...\033[0m"
	@-rm -r bin 2> /dev/null || true
	@-rm Starstone.iso 2> /dev/null || true
	@-rm initrd.tar 2> /dev/null || true

include apps/app/Makefile
