#include "console.h"
#include "intr.h"
#include "clock.h"
#include "stdio.h"
#include "pmm.h"
#include "mem.h"
#include "interrupt.h"
#include "thread.h"
#include "ide.h"
#include "fs.h"
#include "proc.h"
#include "keyboard.h"
#include "proc.h"
#include "tss.h"
#include "ioqueue.h"

extern struct thread_struct* main_thread;

void kthread_a(void *);
void kthread_b(void *b);
void uprog_a(void);
void uprog_b(void);
int test_var_a = 0, test_var_b = 0;

int main()
{
    // console initialization
    cons_init();
    pmm_init();
    mem_init();
    clock_init();
    kthread_init();
    keyboard_init();
    idt_init();
    tss_init();
    
    thread_start("kthread_a", 31, kthread_a, "argA");
    thread_start("kthread_b", 31, kthread_b, "argB");
    //process_execute(uprog_a, "user_prog_a");
    //process_execute(uprog_b, "user_prog_b");

    intr_enable();

	//ide_init();

	//filesys_init();

    while (1) {
		kprintf_lock("Main ");
		//thread_block(TASK_BLOCKED);
		//thread_yield();
    }
    return 0;
}

void kthread_a(void* arg) 
{
    char* para = (char *)arg;
	//void* addr = sys_malloc(512);
	//void* addr1 = sys_malloc(512);
    while (1) {
		//kprintf_lock("%s ", para);
		//kprintf_lock("addr is 0x%x\n", addr);
		//kprintf_lock("addr1 is 0x%x\n", addr);
        kprintf_lock("v_a: 0x\n", test_var_a);
        //enum intr_status old_status = intr_disable();
        //if (!ioqueue_empty(&kbd_buf)) {
        //    kputs(para);
        //    char byte = ioqueue_getchar(&kbd_buf);
        //    kputchar(byte);
        //}
        //set_intr_status(old_status);
    }
}

void kthread_b(void* arg)
{
    char* para = (char *)arg;
	//void* addr = sys_malloc(63);
    while (1) {
    	//kprintf_lock("%s ", para);
		//kprintf_lock("addr is 0x%x\n", addr);
        kprintf_lock("v_b: 0x\n", test_var_b);
        //enum intr_status old_status = intr_disable();
        //if (!ioqueue_empty(&kbd_buf)) {
        //    kputs(para);
        //    char byte = ioqueue_getchar(&kbd_buf);
        //    kputchar(byte);
        //}
        //set_intr_status(old_status);
    }
}

void uprog_a(void)
{
    while(1)
        test_var_a++;
}

void uprog_b(void)
{
    while (1)
        test_var_b++;
}
