#include "defs.h"
#include "x86.h"
#include "console.h"
#include "string.h"

#include "sync.h"

#define VGA_BUF 0xc00b8000
#define CRT_ROWS 25
#define CRT_COLS 80
#define CRT_SIZE (CRT_ROWS * CRT_COLS)

static uint16_t* crt_buf;
static uint16_t crt_pos;
static uint16_t addr_6845;
struct lock console_lock;


/* TEXT-mode - VGA display output */
static void
vga_init()
{
    // always make gs is right
    //asm volatile ("movw $0x18, %ax; movw %ax, %gs;");
    crt_buf = (uint16_t *)VGA_BUF;
    addr_6845 = 0x03d4;

    // Extract cursor location
    uint16_t pos;
    outb(addr_6845, 0xe);
    pos = inb(addr_6845 + 1) << 8; 
    outb(addr_6845, 0xf);
    pos |= inb(addr_6845 + 1);
    crt_pos = pos;
}

/* vga_putc - print character to console */
static void 
vga_putc(int c)
{
    // set green characters on black background
    if (!(c & ~0xff))
        c |= 0x0200;

    switch (c & 0xff) {
        case '\b':
            if (crt_pos > 0) {
                crt_pos--;
                crt_buf[crt_pos] = (c & ~0xff) | ' ';
            }
            break;
        case '\n':
            crt_pos += CRT_COLS;
        case '\r':
            crt_pos -= (crt_pos % CRT_COLS);
            break;
        default:
            crt_buf[crt_pos++] = c; // write the character
            break;
    }

    // roll screen in case
    if (crt_pos >= CRT_SIZE) {
        int i;
        memmove(crt_buf, crt_buf + CRT_COLS, (CRT_SIZE - CRT_COLS) * sizeof(uint16_t));
        for (i = CRT_SIZE - CRT_COLS; i < CRT_SIZE; i++) {
            crt_buf[i] = 0x0700 | ' ';
        }
        crt_pos -= CRT_COLS;
    }

    // move that little blinky thing
    outb(addr_6845, 14);
    outb(addr_6845 + 1, crt_pos >> 8);
    outb(addr_6845, 15);
    outb(addr_6845 + 1, crt_pos);
}

/* cons_init - initializes the console devices */
void cons_init()
{
    vga_init();
    lock_init(&console_lock);
}

void cons_putc(int c)
{
    vga_putc(c);
}

void cons_putc_lock(int c)
{
    lock_acquire(&console_lock);
    vga_putc(c);
    lock_release(&console_lock);
}
