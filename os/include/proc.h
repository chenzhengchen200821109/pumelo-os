#ifndef _PROC_H
#define _PROC_H

#include "defs.h"
#include "list.h"
#include "bitmap.h"
#include "mem.h"
#include "thread.h"

struct task_struct
{
	char* self_kstack;
	enum task_status status;
	struct thread_context context;
	thread_func* func;
	void* func_arg;
	struct trapframe* tf;
	uint8_t priority;
	uint32_t elapsed_ticks;
	struct list_entry general_tag;
	struct list_entry all_list_tag;
	uint32_t* pgdir;
	struct virtual_addr userprog_vaddr;
	char name[16];
	uint32_t stack_magic;
};

#endif
