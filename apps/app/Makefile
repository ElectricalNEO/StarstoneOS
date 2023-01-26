
ROOT ?= ../..

$(ROOT)/bin/apps/app.bin: $(ROOT)/apps/app/main.asm
	@echo "\033[36;1mCOMPILING $@...\033[0m"
	@mkdir -p $(@D)
	@nasm -f bin $< -o $@
