#include "task_manager.h"
#include "../terminal.h"
#include "../tar.h"
#include "../initrd.h"
#include "pit.h"
#include "multitasking.h"
#include "../memory/page_frame_allocator.h"
#include "../memory/memory.h"

void task_manager() {
	
	lock_task();
	struct terminal* term = create_terminal(_framebuffer, tar_open_file((void*)_initrd->address, "zap-light24.psf"));
	if(!term) terminate_current_task();
	uint64_t mem = get_memory_size(0) / 1048576;
	unlock_task();
	while(1) {
		lock_task();
		term->clear(term);
		term->printf(term, "Task Manager\n\nTotal memory: %d MB\nFree memory: %d MB\nUsed memory: %d MB\n\nTasks running:\n\n", mem, get_free_memory() / 1048576, get_used_memory() / 1048576);
		volatile struct task_list_node* node = &task_list;
		while(1) {
			
			term->printf(term, "%s\n", node->data->name);
			node = node->next;
			if(node == &task_list) break;
			
		}
		unlock_task();
		sleep(2000);
		
	}
	while(1);
	
}
