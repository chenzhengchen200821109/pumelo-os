#include "console.h"
#include "intr.h"
#include "clock.h"
#include "stdio.h"
#include "pmm.h"
#include "mem.h"
#include "interrupt.h"
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

    kthread_init();

    thread_start("kthread_a", 31, kthread_a, "argA");
    thread_start("kthread_b", 20, kthread_b, "argB");

    asm volatile ("sti");

    uint32_t counter;

    while (1) {
        asm volatile ("cli");
        kprintf("Main\n");
        asm volatile ("sti");
    }
    return 0;
}

void kthread_a(void* arg) 
{
    char* para = arg;
    while (1) {
        //asm volatile ("cli");
        //kprintf("%s\n", para);
        cons_putc_lock('A');
        cons_putc_lock('\n');
        //asm volatile ("sti");
    }
}

void kthread_b(void* arg)
{
    char* para = arg;
    while (1) {
        //asm volatile ("cli");
        //kprintf("%s\n", para);
        cons_putc_lock('B');
        cons_putc_lock('\n');
        //asm volatile ("sti");
    }
}
