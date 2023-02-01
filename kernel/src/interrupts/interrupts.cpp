#include <TextRenderer.hpp>
#include <interrupts/pic.hpp>
#include <interrupts/interrupts.hpp>

using namespace interrupts;

__attribute__((interrupt)) void interrupts::int_ignore(void* frame) {}

__attribute__((interrupt)) void interrupts::int_06(void* frame) {
	
	TextRenderer::clear();
	TextRenderer::puts("!!!!! ERROR !!!!!\n");
	TextRenderer::puts("Invalid opcode fault occured!");
	while(1);
	
}

__attribute__((interrupt)) void interrupts::int_08(void* frame) {
	
	TextRenderer::clear();
	TextRenderer::puts("!!!!! ERROR !!!!!\n");
	TextRenderer::puts("Double fault occured!");
	while(1);
	
}

__attribute__((interrupt)) void interrupts::int_0d(void* frame) {
	
	TextRenderer::clear();
	TextRenderer::puts("!!!!! ERROR !!!!!\n");
	TextRenderer::puts("General protection fault occured!");
	while(1);
	
}

__attribute__((interrupt)) void interrupts::int_0e(void* frame) {
	
	TextRenderer::clear();
	TextRenderer::puts("!!!!! ERROR !!!!!\n");
	TextRenderer::puts("Page fault occured!");
	while(1);
	
}
