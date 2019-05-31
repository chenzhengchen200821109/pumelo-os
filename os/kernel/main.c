#include "init.h"
#include "stdio.h"

extern struct thread_struct* main_thread;

void kthread_a(void *);
void kthread_b(void *b);
void uprog_a(void);
void uprog_b(void);
int test_var_a = 0, test_var_b = 0;

int main(void)
{
    init_all();
    
    thread_start("kthread_a", 31, kthread_a, "argA");
    thread_start("kthread_b", 31, kthread_b, "argB");
    process_execute(uprog_a, "user_prog_a");
    process_execute(uprog_b, "user_prog_b");

    intr_enable();

	//ide_init();

	//filesys_init();

    while (1) {
		kprintf("Main ");
    }
    return 0;
}

void kthread_a(void* arg) 
{
    while (1) {
        kprintf_lock("v_a: %d ", test_var_a);
    }
}

void kthread_b(void* arg)
{
    while (1) {
        kprintf_lock("v_b: %d ", test_var_b);
    }
}

void uprog_a(void)
{
    while(1) {
        test_var_a++;
    }
}

void uprog_b(void)
{
    while (1) {
        test_var_b++;
    }
}
