
ROOT ?= ../..

$(ROOT)/bin/apps/app: $(ROOT)/apps/app/main.asm
	@echo "\033[36;1mCOMPILING $@.o...\033[0m"
	@mkdir -p $(@D)
	@nasm -f elf64 $< -o $@.o
	@echo "\033[35;1mLINKING $@...\033[0m"
	@ld $@.o -o $@
