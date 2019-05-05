#include "sync.h"
#include "assert.h"
#include "intr.h"

void sema_init(struct semaphore* sema, uint8_t value)
{
	sema->value = value;
	list_init(&sema->waiters);
}

void lock_init(struct lock* lk)
{
	lk->holder = NULL;
	lk->number = 0;
	spinlock_init(&lk->lock);
}

void sema_down(struct semaphore* sema)
{
	enum intr_status old_status = intr_disable();
	while (sema->value == 0) {
		assert(!list_find(&sema->waiters, &running_thread()->general_tag));
		if (list_find(&sema->waiters, &running_thread()->general_tag))
			panic("sema_down: thread blocked has been in waiter list");
		list_append(&sema->waiters, &running_thread()->general_tag);
		thread_block(TASK_BLOCKED);
	}
	sema->value--;
	assert(sema->value == 0);
	set_intr_status(old_status);
}

void sema_up(struct semaphore* sema)
{
	enum intr_status old_status = intr_disable();
	assert(sema->value == 0);
	if (!list_empty(&sema->waiters)) {
		struct thread_struct* thread_blocked = to_struct(list_pop(&sema->waiters), struct thread_struct, general_tag);
		thread_unblock(thread_blocked);
	}
	sema->value++;
	assert(sema->value == 1);
	set_intr_status(old_status);
}

void lock_acquire(struct lock* lk)
{
	if (lk->holder != running_thread()) {
		spinlock_acquire(&lk->lock);
		lk->holder = running_thread();
		assert(lk->number == 0);
		lk->number = 1;
	} else 
		lk->number++;
}

void lock_release(struct lock* lk)
{
	assert(lk->holder == running_thread());
	if (lk->number > 1) {
		lk->number--;
		return;
	}
	assert(lk->number == 1);
	lk->holder = NULL;
	lk->number = 0;
	spinlock_release(&lk->lock);
}
