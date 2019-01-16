#ifndef _CLOCK_H__
#define _CLOCK_H__

#include "defs.h"

extern volatile size_t ticks;

void clock_init(void);

#endif /* _CLOCK_H__ */

