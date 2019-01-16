#include "defs.h"
#include "stdio.h"
#include "intr.h"

static int is_panic = 0;

/* *
 * __panic - __panic is called on unresolvable fatal errors. it prints
 * "panic: 'message'", and then enters the kernel monitor.
 * */
void
__panic(const char *file, int line, const char* func, const char *fmt, ...) {
    if (is_panic) {
        goto panic_dead;
    }
    is_panic = 1;

    // print the 'message'
    va_list ap;
    va_start(ap, fmt);
    kprintf("kernel panic at %s:%d:%s\n", file, line, func);
    kprintf(fmt, ap);
    kprintf("\n");
    va_end(ap);

panic_dead:
    intr_disable();
    while (1) {
        ;
    }
}

/* __warn - like panic, but don't */
void
__warn(const char *file, int line, const char* func, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    kprintf("kernel warning at %s:%d:%s\n", file, line, func);
    kvprintf(fmt, ap);
    kprintf("\n");
    va_end(ap);
}

int is_kernel_panic(void) {
    return is_panic;
}

