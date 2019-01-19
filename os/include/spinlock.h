#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include "defs.h"

struct spinlock
{
    uint32_t locked;  // Is the lock held?
};

void initlock(struct spinlock* lk);
void acquire(struct spinlock* lk);
void release(struct spinlock* lk);
void pushcli(void);
void popcli(void);
int holding(struct spinlock* lk);

#endif
