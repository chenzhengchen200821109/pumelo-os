#include "intr.h"
#include "x86.h"
#include "stdio.h"
#include "kernel.h"
#include "clock.h"
#include "ide.h"
#include "keyboard.h"

#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1

#define IDT_DESC_CNT 0x32

typedef uintptr_t intr_handler;

/* interrupt descriptor */
struct intr_descriptor
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t unused;
    uint8_t attributes;
    uint16_t offset_high;
};

static struct intr_descriptor idt[IDT_DESC_CNT];
extern intr_handler intr_entry_table[IDT_DESC_CNT];

static void lidt(struct intr_descriptor *p, int size)
{
    volatile uint16_t pd[3];

    pd[0] = size - 1;
    pd[1] = (uint32_t)p;
    pd[2] = (uint32_t)p >> 16;

    asm volatile ("lidt (%0)" : : "r"(pd));
}

static void make_intr_descriptor(struct intr_descriptor* pdesc, uint8_t attr, intr_handler func)
{
    pdesc->offset_low = (uint32_t)func & 0x0000ffff;
    pdesc->selector = SELECTOR_KERNEL_CODE;
    pdesc->unused = 0;
    pdesc->attributes = attr;
    pdesc->offset_high = ((uint32_t)func & 0xffff0000) >> 16;
}

void general_intr_handler(uint8_t vecno)
{
    if (vecno == 0x27 || vecno == 0x2f)
        return;
    if (vecno == 14) { // page fault
        int page_fault_vaddr = 0;
        /* cr2 saved the linear address which caused page fault */
        asm volatile ("movl %%cr2, %0" : "=r" (page_fault_vaddr));
        kprintf("page fault vaddr is 0x%x\n", page_fault_vaddr);
        while (1)
            ;
    } else if (vecno == 32) {
        //kprintf("timer interrupt: %d\n", ticks);
        timer_intr_handler();
    } else if (vecno == 46 || vecno == 47) {
		//kprintf("disk interrupt occurred\n");
		hd_intr_handler(vecno);
	} else if (vecno == 33) {
		kprintf("keyboard interrupt occurred\n");
		keyboard_intr_handler();
	} else {
        kprintf("int vector no: %d\n", vecno);
    }
}

static void idt_desc_init()
{
    kprintf("idt_desc_init start\n");
    int i;
    for (i = 0; i < IDT_DESC_CNT; i++)
        make_intr_descriptor(&idt[i], IDT_DESC_ATTR_DPL0, intr_entry_table[i]);
    kprintf("idt_desc_init done\n");
}

/* 8259A */
static void pic_init()
{
    kprintf("pic_init start\n");
    outb(PIC_M_CTRL, 0x11);
    outb(PIC_M_DATA, 0x20);

    outb(PIC_M_DATA, 0x04);
    outb(PIC_M_DATA, 0x01);

    outb(PIC_S_CTRL, 0x11);
    outb(PIC_S_DATA, 0x28);

    outb(PIC_S_DATA, 0x02);
    outb(PIC_S_DATA, 0x01);

    // only enable clock interrupt now
    outb(PIC_M_DATA, 0xfe);
    outb(PIC_S_DATA, 0xff);

	// enable clock and keyboard interrupt
	outb(PIC_M_DATA, 0xfc);
	outb(PIC_S_DATA, 0xff);

    // enable disk interrupt now
    outb(PIC_M_DATA, 0xf8);
    outb(PIC_S_DATA, 0xbf);

    kprintf("pic_init done");
}

void idt_init()
{
    kprintf("idt_init start\n");
    idt_desc_init();
    pic_init(); 

    /* load idt */
    lidt(idt, sizeof(idt));

    kprintf("idt_init done\n");
}
