#include "defs.h"
#include "clock.h"
#include "trap.h"
#include "x86.h"
#include "stdio.h"
#include "console.h"
#include "kernel.h"
#include "picirq.h"

#define TICK_NUM 100

static void print_ticks() {
    kprintf("%d ticks\n", TICK_NUM);
}

/* *
 * Interrupt descriptor table:
 * Must be built at run time because shifted function addresses can't
 * be represented in relocation records.
 * */

/* interrupt gate descriptor */
struct intr_gate_desc
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t unused;
    uint8_t attributes;
    uint16_t offset_high;
};
static struct intr_gate_desc idt[256] = {{0}};
static struct pseudodesc idt_pd = 
{
    sizeof(idt) - 1, (uintptr_t)idt
};
extern uintptr_t __vectors[];

static void make_intr_gate_desc(struct intr_gate_desc* pdesc, uint8_t attr, uintptr_t func)
{
    pdesc->offset_low = (uint32_t)func & 0x0000ffff;
    pdesc->selector = SELECTOR_KERNEL_CODE;
    pdesc->unused = 0;
    pdesc->attributes = attr;
    pdesc->offset_high = ((uint32_t)func & 0xffff0000) >> 16;
}

static void idt_gate_desc_init()
{
    int i;
    for (i = 0; i < sizeof(idt)/sizeof(struct intr_gate_desc); i++)
        make_intr_gate_desc(&idt[i], IDT_DESC_ATTR_DPL0, __vectors[i]);
    kputs("idt_desc_init done");
}

/* 8259A */
//static void pic_init()
//{
//    outb(PIC_M_CTRL, 0x11);
//    outb(PIC_M_DATA, 0x20);

//    outb(PIC_M_DATA, 0x04);
//    outb(PIC_M_DATA, 0x01);

//    outb(PIC_S_CTRL, 0x11);
//    outb(PIC_S_DATA, 0x28);

//    outb(PIC_S_DATA, 0x02);
//    outb(PIC_S_DATA, 0x01);

//    outb(PIC_M_DATA, 0xfe);
//    outb(PIC_S_DATA, 0xff);

//    kputs("pic_init done");
//}

/* idt_init - initialize IDT to each of the entry points in kern/trap/vectors.S */
void
idt_init(void) {
    kprintf("idt_init start\n");
    idt_gate_desc_init();
    //pic_init();
    lidt(&idt_pd);
    kprintf("idt_init done\n");
}

/* trap_dispatch - dispatch based on what type of trap occurred */
static void
trap_dispatch(struct trapframe *tf) {
    char c;

    switch (tf->tf_trapno) {
    case IRQ_OFFSET + IRQ_TIMER:   // clock interrupt
        ticks++;
        if (ticks % TICK_NUM == 0)
            print_ticks();
        break;
    default:
        break;
    }
    // used for debug
    //kprintf("timer interrupt occur\n");
}

/* *
 * trap - handles or dispatches an exception/interrupt. if and when trap() returns,
 * the code in kern/trap/trapentry.S restores the old CPU state saved in the
 * trapframe and then uses the iret instruction to return from the exception.
 * */
void
trap_handler(struct trapframe *tf) {
    // dispatch based on what type of trap occurred
    trap_dispatch(tf);
}

