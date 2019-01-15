#include "console.h"
#include "intr.h"
#include "stdio.h"

int main()
{
    // console initialization
    cons_init();
    idt_init();

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
