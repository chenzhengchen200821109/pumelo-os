#ifndef _SYNC_H
#define _SYNC_H

#include "spinlock.h"
#include "thread.h"
#include "defs.h"
#include "list.h"

struct semaphore
{
	uint8_t value;
	struct list waiters;
};

struct lock
{
	struct semaphore semaphore;
	struct thread_struct* holder;
	uint32_t number;
};

//struct lock
//{
//	struct spinlock lk;
//	struct thread* holder;
//	uint32_t number;
//};

void sema_init(struct semaphore* sema, uint8_t value);
void sema_down(struct semaphore* sema);
void sema_up(struct semaphore* sema);
void lock_init(struct lock* lk);
void lock_acquire(struct lock* lk);
void lock_release(struct lock* lk);

#endif
