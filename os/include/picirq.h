#ifndef _PICIRQ_H__
#define _PICIRQ_H__

void pic_init(void);
void pic_enable(unsigned int irq);

#define IRQ_OFFSET      32

#endif /* _PICIRQ_H__ */

