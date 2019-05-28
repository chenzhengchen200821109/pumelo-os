#ifndef _PROC_H
#define _PROC_H

#include "defs.h"
#include "list.h"
#include "bitmap.h"
#include "mem.h"
#include "thread.h"

#define EFLAGS_MBS (1 << 1)
#define EFLAGS_IF_1 (1<< 9)
#define EFLAGS_IF_0 0
#define EFLAGS_IOPL_3 (3 << 12)
#define EFLAGS_IOPL_0 (0 << 12)

#define USER_VADDR_START 0x8048000
#define USER_STACK3_VADDR (0xc0000000 - 0x1000)

#define DEFAULT_PRIO 30

//struct task_struct
//{
//	char* self_kstack;
//	enum task_status status;
//	struct thread_context context;
//	thread_func* func;
//	void* func_arg;
//	struct trapframe* tf;
//	uint8_t priority;
//	uint32_t elapsed_ticks;
//	struct list_entry general_tag;
//	struct list_entry all_list_tag;
//	uint32_t* pgdir;
//	struct virtual_addr userprog_vaddr;
//	char name[16];
//	uint32_t stack_magic;
//};

void process_execute(void* filename, char* name);

#endif
