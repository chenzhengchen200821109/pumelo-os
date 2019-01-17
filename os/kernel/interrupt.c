#include "intr.h"
#include "x86.h"
#include "stdio.h"
#include "kernel.h"

#define PIC_M_CTRL 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CTRL 0xa0
#define PIC_S_DATA 0xa1

#define IDT_DESC_CNT 0x21

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

static void make_intr_descriptor(struct intr_descriptor* pdesc, uint8_t attr, intr_handler func)
{
    pdesc->offset_low = (uint32_t)func & 0x0000ffff;
    pdesc->selector = SELECTOR_KERNEL_CODE;
    pdesc->unused = 0;
    pdesc->attributes = attr;
    pdesc->offset_high = ((uint32_t)func & 0xffff0000) >> 16;
}

static void idt_desc_init()
{
    int i;
    for (i = 0; i < IDT_DESC_CNT; i++)
        make_intr_descriptor(&idt[i], IDT_DESC_ATTR_DPL0, intr_entry_table[i]);
    kputs("idt_desc_init done");
}

/* 8259A */
static void pic_init()
{
    outb(PIC_M_CTRL, 0x11);
    outb(PIC_M_DATA, 0x20);

    outb(PIC_M_DATA, 0x04);
    outb(PIC_M_DATA, 0x01);

    outb(PIC_S_CTRL, 0x11);
    outb(PIC_S_DATA, 0x28);

    outb(PIC_S_DATA, 0x02);
    outb(PIC_S_DATA, 0x01);

    outb(PIC_M_DATA, 0xfe);
    outb(PIC_S_DATA, 0xff);

    kputs("pic_init done");
}

void idt_init()
{
    kputs("idt_init start");
    idt_desc_init();
    pic_init(); 

    /* load idt */
    uint64_t idt_operand = ((sizeof(idt) - 1) | ((uint64_t)((uint32_t)idt << 16)));
    asm volatile ("lidt %0" : : "m"(idt_operand));
    asm volatile ("sti");

    kputs("idt_init done");
}