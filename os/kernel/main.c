#include "console.h"
#include "intr.h"
#include "picirq.h"
#include "trap.h"
#include "clock.h"
#include "stdio.h"
#include "pmm.h"
#include "mem.h"

int main()
{
    // console initialization
    cons_init();
    kprintf("%x\n", *(uint32_t *)0xc0000d00);
    pic_init();
    idt_init();
    clock_init();
    // reload gdt
    pmm_init(); 
    mem_init();
    void* addr = kmalloc_pages(3);
    kprintf("kmalloc_pages start vaddr is: 0x%x\n", (uint32_t)addr);
    //intr_enable();
    while (1)
        ;
    return 0;
}
