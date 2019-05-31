#include "console.h"
#include "interrupt.h"

void init_all()
{
   // console initialized
   cons_init(); 
   idt_init();

   mem_init();
   clock_init();
   keyboard_init();

   tss_init();

   //ide_init();
   
   kthread_init();
}
