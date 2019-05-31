#include "thread.h"
#include "string.h"
#include "assert.h"
#include "list.h"
#include "intr.h"
#include "defs.h"
#include "mem.h"
#include "stdio.h"
#include "fs.h"

#define PAGE_SIZE 4096

struct thread_struct* main_thread;
struct list thread_ready_list;
struct list thread_list_all;
static struct list_entry* thread_tag;
struct thread_struct* idle_thread;

extern void switch_to(struct thread_context* from, struct thread_context* to);
//extern void switch_to(struct thread_struct* from, struct thread_struct* to);

static void kernel_thread_entry(thread_func* func, void* func_arg)
{
    func(func_arg);
}

void Thread_init(struct thread_struct* pthread, const char* name, int prio)
{
    memset(pthread, 0, sizeof(*pthread));
    strcpy(pthread->name, name);

    if (pthread == main_thread)
        pthread->status = TASK_RUNNING;
    else
        pthread->status = TASK_READY;

    /* thread's kernel stack */
    pthread->self_kstack = (char *)((char *)pthread + PAGE_SIZE);
    pthread->priority = prio;
    pthread->ticks = prio;
    pthread->elapsed_ticks = 0;
    pthread->pgdir = NULL;
    pthread->stack_magic = 0x19870916;

    /* file system */
	pthread->fd_table[0] = 0;
	pthread->fd_table[1] = 1;
	pthread->fd_table[1] = 2;
	uint8_t fd_idx = 3;
	while (fd_idx < MAX_FILE_OPEN_PER_PROC) {
		pthread->fd_table[fd_idx] = -1;
		fd_idx++;
	}
}

void Thread_create(struct thread_struct* pthread, thread_func* func, void* func_arg)
{
    char* sp = pthread->self_kstack;
    sp = sp - sizeof(struct trapframe);
    pthread->tf = (struct trapframe *)sp;
    //sp = (char *)((unsigned long)sp & ~16L);
    sp = sp - sizeof(struct thread_parameter);
    struct thread_parameter* param = (struct thread_parameter *)sp;
    param->function = func;
    param->arg = func_arg;
    sp = sp - sizeof(void *);
    pthread->self_kstack = sp;
    //kprintf("sp address is 0x%x\n", pthread->self_kstack);
    pthread->context.eip = (void *)kernel_thread_entry;
    pthread->context.esp = (void *)sp;

    //
    //pthread->self_kstack -= sizeof(struct trapframe);
    //pthread->self_kstack -= sizeof(struct thread_context);
    //struct thread_context *kthread_context = (struct thread_context *)pthread->self_kstack;
    //kthread_context->eip = kernel_thread_entry;
    //kthread_context->func = func;
    //kthread_context->func_arg = func_arg;
    //kthread_context->ebp = kthread_context->ebx = kthread_context->esi = kthread_context->edi = 0;
}

/*
 * schedule() invoked by timer_intr_handler().
 */
void schedule()
{
    assert(get_intr_status() == INTR_OFF);

    struct thread_struct* cur = running_thread();

    if (cur->status == TASK_RUNNING) {
        assert(!list_find(&thread_ready_list, &cur->general_tag));
        // already in thread_ready_list
        list_append(&thread_ready_list, &cur->general_tag);
        cur->ticks = cur->priority;
        cur->status = TASK_READY;
    } else {
        // TASK_READY 
    }

    //assert(!list_empty(&thread_ready_list));
	if (list_empty(&thread_ready_list)) {
		thread_unblock(idle_thread);
	}
    thread_tag = NULL;
    thread_tag = list_pop(&thread_ready_list);
    struct thread_struct* next = to_struct(thread_tag, struct thread_struct, general_tag);
    next->status = TASK_RUNNING;
	//
	process_activate(next);
    switch_to(&cur->context, &next->context);
    //switch_to(cur, next);
}

struct thread_struct* running_thread()
{
    uint32_t esp;
    asm volatile ("movl %%esp, %0" : "=g" (esp));
    return (struct thread_struct *)(esp & 0xfffff000);
}

struct thread_struct* thread_start(char* name, int prio, thread_func* func, void* func_arg)
{
    struct thread_struct* thread = (struct thread_struct *)get_kernel_pages(1);

    Thread_init(thread, name, prio);
    Thread_create(thread, func, func_arg);

    assert(!list_find(&thread_ready_list, &thread->general_tag));
    list_append(&thread_ready_list, &thread->general_tag);
    
    assert(!list_find(&thread_list_all, &thread->all_list_tag));
    list_append(&thread_list_all, &thread->all_list_tag);

    // debug
    //list_traversal(&thread_ready_list, (function)list_print, 0);

    return thread;
}

void thread_block(enum task_status stat)
{
    assert((stat == TASK_BLOCKED) || (stat == TASK_WAITING) || (stat == TASK_HANGING));
    enum intr_status old_status = intr_disable();
    struct thread_struct* cur_thread = running_thread();
    cur_thread->status = stat;
    schedule();
    set_intr_status(old_status);
}

void thread_unblock(struct thread_struct* pthread)
{
    enum intr_status old_status = intr_disable();
    assert((pthread->status == TASK_BLOCKED) || (pthread->status == TASK_WAITING) || (pthread->status == TASK_HANGING));
    if (pthread->status != TASK_READY) {
        assert(!list_find(&thread_ready_list, &pthread->general_tag));
        if (list_find(&thread_ready_list, &pthread->general_tag))
            panic("thread_unblock: blocked thread in ready list\n");
        list_push(&thread_ready_list, &pthread->general_tag);
        pthread->status = TASK_READY;
    }
    set_intr_status(old_status);
}

static void Make_Main_Thread()
{
    main_thread = running_thread();
    Thread_init(main_thread, "main", 31);

    assert(!list_find(&thread_list_all, &main_thread->all_list_tag));
    list_append(&thread_list_all, &main_thread->all_list_tag);
}

//struct thread_struct* ide_thread;

static void idle(void* arg)
{
	while (1) {
		thread_block(TASK_BLOCKED);
		//kprintf_lock("idle is working\n");
		//thread_unblock(main_thread);
		asm volatile ("sti; hlt" : : : "memory");
	}
}

void thread_yield()
{
	struct thread_struct* cur = running_thread();
	enum intr_status old_status = intr_disable();
	assert(!list_find(&thread_ready_list, &cur->general_tag));
	list_append(&thread_ready_list, &cur->general_tag);
	cur->status = TASK_READY;
	schedule();
	set_intr_status(old_status);
}

void kthread_init()
{
    kprintf("kthread_init start\n");
    list_init(&thread_ready_list);
    list_init(&thread_list_all);
    Make_Main_Thread();

	idle_thread = thread_start("idle", 10, idle, NULL);

    kprintf("kthread_init done\n");
}
