#include "console.h"
#include "intr.h"
#include "picirq.h"
#include "trap.h"
#include "clock.h"
#include "stdio.h"

int main()
{
    // console initialization
    cons_init();
    pic_init();
    idt_init();
    clock_init();
    intr_enable();

    //while (1);
    //cons_putc('k');
    //cons_putc('e');
    //cons_putc('r');
    //cons_putc('n');
    //cons_putc('e');
    //cons_putc('l');
    //cons_putc('\n');
    //cons_putc('1');
    //cons_putc('2');
    //cons_putc('\b');
    //cons_putc('3');
    kputs("kernel");
    while (1)
        ;
    return 0;
}
