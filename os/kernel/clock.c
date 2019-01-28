#include "x86.h"
#include "stdio.h"
#include "thread.h"
#include "assert.h"

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

#define mil_seconds_per_intr (1000 / IRQ0_FREQUENCY)

volatile size_t ticks;

static void ticks_to_sleep(uint32_t sleep_ticks)
{
	uint32_t start_tick = ticks;

	while (ticks - start_tick < sleep_ticks)
		thread_yield();
}

void mtime_sleep(uint32_t m_seconds)
{
	uint32_t sleep_ticks = DIV_ROUND_UP(m_seconds, mil_seconds_per_intr);
	assert(sleep_ticks > 0);
	ticks_to_sleep(sleep_ticks);
}

void timer_intr_handler() 
{
    struct thread_struct* cur_thread = running_thread();
    assert(cur_thread->stack_magic == 0x19870916);
    cur_thread->elapsed_ticks++;
    ticks++;
    if (cur_thread->ticks == 0)
        schedule();
    else
        cur_thread->ticks--;
}

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
clock_init(void) 
{
    // set 8253 timer-chip
    // In order to debug, do not set frequency too large !!!
    //set_timer_frequency(COUNTER0_PORT, COUNTER0_NO, READ_WRITE_LOCK, COUNTER0_MODE, COUNTER0_VALUE); 
    // initialize time counter 'ticks' to zero
    ticks = 0;
    //register_handler(0x20, (intr_handler)timer_intr_handler);
    kprintf("clock_init done\n");
}

