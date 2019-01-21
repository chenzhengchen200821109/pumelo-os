#ifndef _CLOCK_H__
#define _CLOCK_H__

#include "defs.h"

extern volatile size_t ticks;

void clock_init(void);
void timer_intr_handler();

#endif /* _CLOCK_H__ */

