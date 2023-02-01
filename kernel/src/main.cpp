#include <psf.hpp>
#include <Tar.hpp>
#include <Initrd.hpp>
#include <string.hpp>
#include <memory/Heap.hpp>
#include <constructors.hpp>
#include <hardware/pit.hpp>
#include <TextRenderer.hpp>
#include <interrupts/idt.hpp>
#include <memory/MemoryMap.hpp>
#include <memory/PageTable.hpp>
#include <syscalls/syscalls.hpp>
#include <multitasking/Scheduler.hpp>
#include <memory/PageFrameAllocator.hpp>
#include <multitasking/ProgramLoader.hpp>

extern "C" void kmain(Framebuffer* framebuffer, Initrd* initrd, memory::MemoryMap* mmap) {
	
	callConstructors();
	
	Tar initrdTar(initrd->address);
	PSF2_Header* font = (PSF2_Header*)initrdTar.findFile("zap-light24.psf");
	if(!font) return;
	
	TextRenderer::init(framebuffer, font);
	TextRenderer::puts("Starstone 1.0 Ahlspiess - Booting up...\n");
	
	if(memory::PageFrameAllocator::init(framebuffer, initrd, mmap)) {
		
		TextRenderer::puts("ERROR: Failed to initialize page frame allocator!\nOut of memory.");
		return;
		
	}
	
	if(memory::Heap::init(0xffffff0100000000)) {
	
		TextRenderer::puts("ERROR: Failed to initialize heap!\nOut of memory.");
		return;
	
	}
	
	interrupts::IDT idt(0xffffff7ffffff000);
	if(!idt.isGood()) {
		
		TextRenderer::puts("ERROR: Failed to create IDT!\nOut of memory.");
		return;
		
	}
	
	syscalls::init_syscall();
	
	hardware::PIT::setDivisor(32768);
	
	TextRenderer::puts("Initialization complete.\n");
	
	if(multitasking::Scheduler::startTask(0, 0, 0, 0, &memory::kernelPageTable)) { // these values will be overwritten anyway
		
		TextRenderer::puts("Failed to start kernel task!\nOut of memory.");
		return;
		
	}
	
	void* helloworld = initrdTar.findFile("helloworld");
	if(!helloworld) {
		
		TextRenderer::puts("No such file \"helloworld\" on the initial RAM disk!");
		return;
		
	}
	
	if(multitasking::ProgramLoader::loadELF(helloworld)) {
	
		TextRenderer::puts("Failed to start task helloworld!");
		return;
	
	}
	
	idt.activate();
	
	asm("mov rax, 0x0123456789abcdef\njmp $");
	
}
