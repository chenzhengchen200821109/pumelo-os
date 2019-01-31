#include "defs.h"
#include "stdio.h"
#include "console.h"
#include "sync.h"

extern struct lock console_lock;

/* HIGH level console I/O */

/* *
 * cputch - writes a single character @c to stdout, and it will
 * increace the value of counter pointed by @cnt.
 * */
static void
cputch(int c, int *cnt) {
    cons_putc(c);
    (*cnt)++;
}

static void
cputch_lock(int c, int* cnt)
{
	lock_acquire(&console_lock);
	cons_putc(c);
	(*cnt)++;
	lock_release(&console_lock);
}

/* *
 * vcprintf - format a string and writes it to stdout
 *
 * The return value is the number of characters which would be
 * written to stdout.
 *
 * Call this function if you are already dealing with a va_list.
 * Or you probably want cprintf() instead.
 * */
int
kvprintf(const char *fmt, va_list ap) {
    int cnt = 0;
    vprintfmt((void*)cputch, &cnt, fmt, ap);
    return cnt;
}

int
kvprintf_lock(const char* fmt, va_list ap)
{
	int cnt = 0;
	vprintfmt((void *)cputch_lock, &cnt, fmt, ap);
	return cnt;
}

/* *
 * cprintf - formats a string and writes it to stdout
 *
 * The return value is the number of characters which would be
 * written to stdout.
 * */
int
kprintf(const char *fmt, ...) {
    va_list ap;
    int cnt;
    va_start(ap, fmt);
    cnt = kvprintf(fmt, ap);
    va_end(ap);
    return cnt;
}

int kprintf_lock(const char* fmt, ...)
{
	va_list ap;
	int cnt;
	va_start(ap, fmt);
	cnt = kvprintf_lock(fmt, ap);
	va_end(ap);
	return cnt;
}

/* cputchar - writes a single character to stdout */
void
kputchar(int c) {
    cons_putc(c);
}

void kputchar_lock(int c)
{
	cons_putc_lock(c);
}

/* *
 * cputs- writes the string pointed by @str to stdout and
 * appends a newline character.
 * */
int
kputs(const char *str) {
    int cnt = 0;
    char c;
    while ((c = *str++) != '\0') {
        cputch(c, &cnt);
    }
    cputch('\n', &cnt);
    return cnt;
}

int kputs_lock(const char* str)
{
	int cnt = 0;
	char c;
	lock_acquire(&console_lock);
	while ((c = *str++) != '\0') {
		cputch(c, &cnt);
	}
	cputch('\n', &cnt);
	lock_release(&console_lock);
	return cnt;
}
