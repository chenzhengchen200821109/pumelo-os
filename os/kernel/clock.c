#include "x86.h"
#include "trap.h"
#include "stdio.h"
#include "picirq.h"

/* *
 * Frequency of all three count-down timers; (TIMER_FREQ/freq)
 * is the appropriate count to generate a frequency of freq Hz.
 * */

#define INPUT_FREQUENCY      1193182
#define IRQ0_FREQUENCY       100
#define COUNTER0_VALUE       (INPUT_FREQUENCY / IRQ0_FREQUENCY)
#define COUNTER0_PORT        0x40             // control register
#define COUNTER0_NO          0                // select counter 0
#define COUNTER0_MODE        2                // mode 2, rate generator
#define READ_WRITE_LOCK      3                // r/w counter 16 bits
#define COUNTER0_CONTROL     0x43

volatile size_t ticks;

static void set_timer_frequency(uint8_t port, uint8_t no,  uint8_t rwl, uint8_t mode, uint16_t value)
{
    outb(COUNTER0_CONTROL, (uint8_t)(no << 6 | rwl << 4 | mode << 1));
    outb(COUNTER0_PORT, (uint8_t)value);
    outb(COUNTER0_PORT, (uint8_t)value >> 8);
}

/* *
 * clock_init - initialize 8253 clock to interrupt 100 times per second,
 * and then enable IRQ_TIMER.
 * */
void
clock_init(void) {
    // set 8253 timer-chip
    set_timer_frequency(COUNTER0_PORT, COUNTER0_NO, READ_WRITE_LOCK, COUNTER0_MODE, COUNTER0_VALUE); 

    // initialize time counter 'ticks' to zero
    ticks = 0;

    kprintf("setup timer interrupts\n");
    // enable clock interrupt
    pic_enable(IRQ_TIMER);
}

