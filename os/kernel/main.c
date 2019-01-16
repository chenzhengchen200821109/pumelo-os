#include "console.h"
#include "intr.h"
#include "picirq.h"
#include "trap.h"
#include "clock.h"
#include "stdio.h"
#include "pmm.h"

int main()
{
    // console initialization
    cons_init();
    pic_init();
    idt_init();
    clock_init();
    // reload gdt
    pmm_init(); 
    intr_enable();
    while (1)
        ;
    return 0;
}
