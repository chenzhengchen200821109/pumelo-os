#ifndef _ASSERT_H__
#define _ASSERT_H__

#include "defs.h"

void __warn(const char *file, int line, const char *fmt, ...);
void __panic(const char *file, int line, const char *fmt, ...) __attribute__((noreturn));

#define warn(...)                                       \
    __warn(__FILE__, __LINE__, __func__,  __VA_ARGS__)

#define panic(...)                                      \
    __panic(__FILE__, __LINE__, __func__, __VA_ARGS__)

#define assert(x)                                       \
    do {                                                \
        if (!(x)) {                                     \
            panic("assertion failed: %s", #x);          \
        }                                               \
    } while (0)

// static_assert(x) will generate a compile-time error if 'x' is false.
#define static_assert(x)                                \
    switch (x) { case 0: case (x): ; }

#endif /* _ASSERT_H__ */

