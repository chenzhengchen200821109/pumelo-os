#ifndef _STDIO_H__
#define _STDIO_H__

#include "defs.h"
#include "stdarg.h"

/* kernel/stdio.c */
int kprintf(const char *fmt, ...);
int kprintf_lock(const char* fmt, ...);
int kvprintf(const char *fmt, va_list ap);
int kvprintf(const char* fmt, va_list ap);
void kputchar(int c);
void kputchar_lock(int c);
int kputs(const char *str);
int kputs_lock(const char* str);

/* kernel/printfmt.c */
void printfmt(void (*putch)(int, void *), void *putdat, const char *fmt, ...);
void vprintfmt(void (*putch)(int, void *), void *putdat, const char *fmt, va_list ap);
int snprintf(char *str, size_t size, const char *fmt, ...);
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap);

#endif /* _STDIO_H__ */

