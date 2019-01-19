#include "console.h"
//#include "intr.h"
#include "picirq.h"
#include "trap.h"
#include "clock.h"
#include "stdio.h"
#include "pmm.h"
#include "mem.h"
#include "thread.h"

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
    asm volatile ("sti");

    //thread_start("kthread_a", 31, kthread_a, "argA");
    //thread_start("kthread_b", 31, kthread_b, "argB");

    while (1) ;
        //kprintf("Main ");
    return 0;
}

void kthread_a(void* arg) 
{
    char* para = arg;
    while (1) {
        kprintf(para);
    }
}

void kthread_b(void* arg)
{
    char* para = arg;
    while (1) {
        kprintf(para);
    }
}
