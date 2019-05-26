#ifndef _THREAD_H
#define _THREAD_H

#include "defs.h"
#include "list.h"
#include "mem.h"

#define MAX_FILES_OPEN_PER_PROC 8

typedef void thread_func(void *);

enum task_status
{
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_WAITING,
    TASK_HANGING,
    TASK_DIED
};

struct trapframe
{
    uint32_t vecno;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t err_code;
    void (*eip)(void);
    uint32_t cs;
    uint32_t eflags;
    void* esp;
    uint32_t ss;
};

struct thread_parameter
{
    const void* function;
    const void* arg;
};

struct thread_context
{
    //uint32_t eip;
    void* eip;
    //uint32_t esp;
    void* esp;
    //uint32_t ebx;
    void* ebx;
    //uint32_t ecx;
    void* ecx;
    //uint32_t edx;
    void* edx;
    //uint32_t esi;
    void* esi;
    //uint32_t edi;
    void* edi;
    //uint32_t ebp;
    void* ebp;

    //uint32_t ebp;
    //uint32_t ebx;
    //uint32_t edi;
    //uint32_t esi;
    //void (*eip)(thread_func* func, void* func_arg);
    //thread_func* func;
    //void* func_arg;
};

struct thread_struct
{
    char* self_kstack;
    enum task_status status;
    struct thread_context context;
    thread_func* func;
    void* func_arg;
    struct trapframe* tf;
    uint8_t priority;
    uint8_t ticks;
    uint32_t elapsed_ticks;
	int32_t fd_table[MAX_FILES_OPEN_PER_PROC];
    struct list_entry general_tag;  // thread_ready_list
    struct list_entry all_list_tag;
    uint32_t* pgdir;
    char name[16];
	// for user processes
	struct virtual_addr userprog_vaddr;
    uint32_t stack_magic;
};

struct thread_struct* running_thread();
void schedule();
struct thread_struct* thread_start(char* name, int prio, thread_func* func, void* func_arg);
void thread_block(enum task_status stat);
void thread_unblock(struct thread_struct* pthread);

void kthread_init();

#endif
