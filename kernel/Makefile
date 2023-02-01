# https://stackoverflow.com/a/18258352
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

ROOT ?= ..

SOURCES = $(call rwildcard, $(ROOT)/kernel/src, *.asm *.cpp)
_objects = $(patsubst $(ROOT)/kernel/src/%.asm, $(ROOT)/bin/kernel/%_asm.o, $(SOURCES))
OBJECTS = $(patsubst $(ROOT)/kernel/src/%.cpp, $(ROOT)/bin/kernel/%_cpp.o, $(_objects))

CPP_FLAGS = -c -m64 -I$(ROOT)/kernel/include -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -masm=intel -fpermissive -O0

default: $(ROOT)/bin/kernel/starkrnl

$(ROOT)/bin/kernel/%_asm.o: $(ROOT)/kernel/src/%.asm
	@echo "\033[36;1mCOMPILING $@...\033[0m"
	@mkdir -p $(@D)
	@nasm -f elf64 $< -o $@

$(ROOT)/bin/kernel/%_cpp.o: $(ROOT)/kernel/src/%.cpp
	@echo "\033[36;1mCOMPILING $@...\033[0m"
	@mkdir -p $(@D)
	@g++ $< -o $@ $(CPP_FLAGS)

$(ROOT)/bin/kernel/interrupts/interrupts_cpp.o: $(ROOT)/kernel/src/interrupts/interrupts.cpp
	@echo "\033[36;1mCOMPILING $@...\033[0m"
	@mkdir -p $(@D)
	@g++ $< -o $@ $(CPP_FLAGS) -mgeneral-regs-only

$(ROOT)/bin/kernel/starkrnl: $(ROOT)/kernel/linker.ld $(OBJECTS)
	@echo "\033[35;1mLINKING $@...\033[0m"
	@ld -o $@ -T $^ --no-relax