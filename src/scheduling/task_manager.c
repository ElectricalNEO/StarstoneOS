#include "task_manager.h"
#include "../terminal.h"
#include "../tar.h"
#include "../initrd.h"
#include "pit.h"
#include "multitasking.h"

void task_manager() {
	
	lock_task();
	struct terminal* term = create_terminal(_framebuffer, tar_open_file((void*)_initrd->address, "zap-light24.psf"));
	if(!term) return;
	unlock_task();
	while(1) {
		lock_task();
		term->clear(term);
		term->puts(term, "Task Manager\n\nTasks running:\n\n");
		struct task_list_node* node = &task_list;
		while(node) {
			
			term->printf(term, "%s\n", node->data->name);
			node = node->next;
			
		}
		unlock_task();
		sleep(2000);
		
	}
	
}
