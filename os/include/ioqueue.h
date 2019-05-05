#ifndef _IOQUEUE_H
#define _IOQUEUE_H

#include "thread.h"
#include "sync.h"

#define BUF_SIZE 64

struct ioqueue
{
	struct lock lock;
	struct task_struct* product;
	struct task_struct* consumer;
	char buf[BUF_SIZE];
	int32_t head;
	int32_t tail;
};

void ioqueue_init(struct ioqueue* ioq);
bool ioqueue_empty(struct ioqueue* ioq);
char ioqueue_getchar(struct ioqueue* ioq);
void ioqueue_putchar(struct ioqueue* ioq, char c);

#endif
