#include "x86.h"
#include "intr.h"

#define EFLAGS_IF 0x00000200
#define GET_EFLAGS(EFLAGS_VAR) asm volatile ("pushfl; popl %0" : "=g" (EFLAGS_VAR))

/* intr_enable - enable irq interrupt */
enum intr_status intr_enable(void) 
{
    enum intr_status old_status;
    if (INTR_ON == get_intr_status()) {
        old_status = INTR_ON;
        return old_status;
    } else {
        old_status = INTR_OFF;
        sti();
        return old_status;
    }
}

/* intr_disable - disable irq interrupt */
enum intr_status intr_disable(void) 
{
    enum intr_status old_status;
    if (INTR_ON == get_intr_status()) {
        old_status = INTR_ON;
        cli();
        return old_status;
    } else {
        old_status = INTR_OFF;
        return old_status;
    }
}

enum intr_status get_intr_status()
{
    uint32_t eflags = 0;
    GET_EFLAGS(eflags);
    return (EFLAGS_IF & eflags) ? INTR_ON : INTR_OFF;
}

enum intr_status set_intr_status(enum intr_status status)
{
    return status & INTR_ON ? intr_enable() : intr_disable();
}
