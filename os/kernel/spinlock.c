#include "spinlock.h"
#include "assert.h"
#include "x86.h"

void spinlock_init(struct spinlock* lk)
{
    lk->locked = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause other
// CPUs to waste time spinning to acquire it.
void spinlock_acquire(struct spinlock* lk)
{
    pushcli();  // disable interrupt to avoid deadlock
    if (holding(lk))
        panic("acquire");

    // The xchg is atomic.
    // It also serializes, so that reads after acquire are
    // not reordered before it.
    while (xchg(&lk->locked, 1) != 0)
        ;
}

// Release the lock
void spinlock_release(struct spinlock* lk)
{
    if (!holding(lk))
        panic("release");
    xchg(&lk->locked, 0);

    popcli();
}

// Check whether this cpu is holding the lock
int holding(struct spinlock* lock)
{
    return lock->locked;
}

void pushcli()
{
    cli();
}

void popcli()
{
    sti();
}
