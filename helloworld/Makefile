# https://stackoverflow.com/a/18258352
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ROOT ?= ..

default: $(ROOT)/bin/helloworld/helloworld

$(ROOT)/bin/helloworld/helloworld.o: $(ROOT)/helloworld/helloworld.asm
	@echo "\033[36;1mCOMPILING $@...\033[0m"
	@mkdir -p $(@D)
	@nasm -f elf64 $< -o $@

$(ROOT)/bin/helloworld/helloworld: $(ROOT)/bin/helloworld/helloworld.o
	@echo "\033[35;1mLINKING $@...\033[0m"
	@ld $< -o $@
