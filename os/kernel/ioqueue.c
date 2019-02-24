#include "ioqueue.h"

void ioqueue_init(struct ioqueue* ioq)
{
	lock_init(&ioq->lock);
	ioq->product = ioq->consumer = NULL;
	ioq->head = ioq->tail = 0;
}

static int32_t next_pos(int32_t pos)
{
	return (pos + 1) % BUF_SIZE;
}

bool ioqueue_empty(struct ioqueue* ioq)
{
	assert(get_intr_status() == INTR_OFF);
	return ioq->head == ioq->tail;
}

bool ioqueue_full(struct ioqueue* ioq)
{
	assert(get_intr_status() == INTR_OFF);
	return next_pos(ioq->head) == ioq->tail;
}

static void ioqueue_wait(struct task_struct** waiter)
{
	assert(*waiter == NULL && waiter != NULL);
	*waiter = running_thread();
	thread_block(TASK_BLOCKED);
}

static void ioqueue_wakeup(struct task_struct** waiter)
{
	assert(*waiter != NULL);
	thread_unblock(*waiter);
	*waiter = NULL;
}

char ioqueue_getchar(struct ioqueue* ioq)
{
	assert(get_intr_status() == INTR_OFF);
	while (ioqueue_empty(ioq)) {
		lock_acquire(&ioq->lock);
		ioqueue_wait(&ioq->consumer);
		lock_release(&ioq->lock);
	}
	char c = ioq->buf[ioq->tail];
	ioq->tail = next_pos(ioq->tail);
	if (ioq->product != NULL) {
		ioqueue_wakeup(&ioq->producer);
	}
	return c;
}

void ioqueue_putchar(struct ioqueue* ioq, char c)
{
	assert(get_intr_status() == INTR_OFF);
	while (ioqueue_full(ioq)) {
		lock_acquire(&ioq->lock);
		ioqueue_wait(&ioq->producer);
		lock_release(&ioq->lock);
	}
	ioq->buf[ioq->head] = c;
	ioq->head = next_pos(ioq->head);
	if (ioq->consumer != NULL) {
		ioqueue_wakeup(&ioq->consumer);
	}
}
