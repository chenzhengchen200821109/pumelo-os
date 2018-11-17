#include <defs.h>
#include <stdio.h>
#include <string.h>
#include <console.h>
#include <kdebug.h>
#include <picirq.h>
#include <trap.h>
#include <clock.h>
#include <intr.h>
#include <pmm.h>
#include <kmonitor.h>


int kern_init(void) __attribute__((noreturn));

//void grade_backtrace(void);
//static void lab1_switch_test(void);

int kern_init(void) 
{
    extern char edata[], end[]; // tools/kernel.ld
    
    memset(edata, 0, end - edata);

    /*
     * console initialization.
     */
    cons_init(); 

    const char* message = "(THU.CST) os is loading ...";
    cprintf("%s\n\n", message);

    print_kerninfo();

    //grade_backtrace();

    /* physical memory management setup */
    pmm_init();                 

    /* interrupt controller initialization */
    pic_init();            

    /* interrupt desriptor table initialization */
    idt_init();                 

    /* clock interrupt controller initialization */
    clock_init();               

    /* IRQ interrupt controller initialization */
    intr_enable();       

    //LAB1: CAHLLENGE 1 If you try to do it, uncomment lab1_switch_test()
    // user/kernel mode switch test
    //lab1_switch_test();

    /* do nothing */
    while (1);
}

void __attribute__((noinline))
grade_backtrace2(int arg0, int arg1, int arg2, int arg3) { // arg0 = 0, arg1 = &arg0, arg2 = 0xffff0000, arg3 = &arg0
    mon_backtrace(0, NULL, NULL);
}

void __attribute__((noinline))
grade_backtrace1(int arg0, int arg1) { // arg0 = 0, arg1 = 0xffff0000
    grade_backtrace2(arg0, (int)&arg0, arg1, (int)&arg1);
}

void __attribute__((noinline))
grade_backtrace0(int arg0, int arg1, int arg2) { // arg0 = 0, arg1 = 0x00100000, arg2 = 0xffff0000
    grade_backtrace1(arg0, arg2); 
}

void
grade_backtrace(void) {
    grade_backtrace0(0, (int)kern_init, 0xffff0000);
}

static void
lab1_print_cur_status(void) {
    static int round = 0;
    uint16_t reg1, reg2, reg3, reg4;
    asm volatile (
            "mov %%cs, %0;"
            "mov %%ds, %1;"
            "mov %%es, %2;"
            "mov %%ss, %3;"
            : "=m"(reg1), "=m"(reg2), "=m"(reg3), "=m"(reg4));
    cprintf("%d: @ring %d\n", round, reg1 & 3);
    cprintf("%d:  cs = %x\n", round, reg1);
    cprintf("%d:  ds = %x\n", round, reg2);
    cprintf("%d:  es = %x\n", round, reg3);
    cprintf("%d:  ss = %x\n", round, reg4);
    round ++;
}

static void
lab1_switch_to_user(void) {
    //LAB1 CHALLENGE 1 : TODO
    asm volatile (
            "sub $0x8, %%esp \n"
            "int %0 \n"
            "mov %%ebp, %%esp"
            :
            : "i"(T_SWITCH_TOU)
            );

}

static void
lab1_switch_to_kernel(void) {
    //LAB1 CHALLENGE 1 :  TODO
    asm volatile (
            "int %0 \n"
            "movl %%ebp, %%esp \n"
            :
            : "i"(T_SWITCH_TOK)
            );
}

static void
lab1_switch_test(void) {
    lab1_print_cur_status();
    cprintf("+++ switch to  user  mode +++\n");
    lab1_switch_to_user();
    lab1_print_cur_status();
    cprintf("+++ switch to kernel mode +++\n");
    lab1_switch_to_kernel();
    lab1_print_cur_status();
}

