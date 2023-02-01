
ROOT = .

default: Starstone.iso

initrd.tar: files/zap-light24.psf bin/helloworld/helloworld
	@echo "\033[33;1mGENERATING $@...\033[0m"
	@tar -cf $@ -C files zap-light24.psf -C ../bin/helloworld helloworld

Starstone.iso: bin/kernel/starkrnl initrd.tar
	@echo "\033[33;1mGENERATING $@...\033[0m"
	@mkdir -p iso/boot/grub
	@cp bin/kernel/starkrnl iso/boot/
	@cp initrd.tar iso/boot/
	@cp grub.cfg iso/boot/grub/
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

include kernel/Makefile
include helloworld/Makefile
