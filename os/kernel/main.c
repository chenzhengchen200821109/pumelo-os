#include "console.h"
#include "intr.h"
#include "clock.h"
#include "stdio.h"
#include "pmm.h"
#include "mem.h"
#include "interrupt.h"
#include "thread.h"
#include "ide.h"

extern struct thread_struct* main_thread;

void kthread_a(void *);
void kthread_b(void *b);

int main()
{
    // console initialization
    cons_init();
    pmm_init();
    mem_init();

    idt_init();
    clock_init();

    kthread_init();

    thread_start("kthread_a", 31, kthread_a, "argA");
    thread_start("kthread_b", 20, kthread_b, "argB");

    asm volatile ("sti");

	ide_init();

    while (1) {
		//kprintf_lock("Main ");
		//thread_block(TASK_BLOCKED);
		//thread_yield();
    }
    return 0;
}

void kthread_a(void* arg) 
{
    char* para = arg;
	void* addr = sys_malloc(33);
    while (1) {
		kprintf_lock("%s ", para);
		kprintf_lock("addr is 0x%x\n", addr);
    }
}

void kthread_b(void* arg)
{
    char* para = arg;
	void* addr = sys_malloc(63);
    while (1) {
    	kprintf_lock("%s ", para);
		kprintf_lock("addr is 0x%x\n", addr);
    }
}
