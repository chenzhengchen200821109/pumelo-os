#include "keyboard.h"
#include "stdio.h"
#include "interrupt.h"
#include "ioqueue.h"
#include "x86.h"

#define KBD_BUF_PORT 0x60

#define ESC       '\033'
#define BACKSPACE '\b'
#define TAB       '\b'
#define ENTER     '\r'
#define DELETE    '\177'

#define char_invisible 0
#define ctrl_l_char    0
#define ctrl_r_char    0
#define shift_l_char   0
#define shift_r_char   0
#define alt_l_char     0
#define alt_r_char     0
#define caps_lock_char 0

#define shift_l_make   0x2a
#define shift_r_make   0x36
#define alt_l_make     0x38
#define alt_r_make     0xe038
#define ctrl_l_make    0x1d
#define ctrl_r_make    0xe01d
#define ctrl_r_break   0xe09d
#define caps_lock_make 0x3a

static bool ctrl_status, shift_status, alt_status, caps_lock_status, ext_scancode;

static char keymap[][2] = 
{
	{0, 				0},
	{ESC,             ESC},
	{'1',			  '!'},
	{'2',			  '@'},
	{'3',			  '#'},
	{BACKSPACE, BACKSPACE},
};

struct ioqueue kbd_buf;

void keyboard_intr_handler()
{
    kputchar('k');
    inb(KBD_BUF_PORT);
    return;
	//bool ctrl_down_last = ctrl_status;
	//bool shift_down_last = shift_status;
	//bool caps_lock_last = caps_lock_status;
	//bool break_code;
	//uint16_t scancode = inb(KBD_BUF_PORT);

	//if (scancode == 0xe0) {
	//	ext_scancode = true;
	//	return;
	//}

	//if (ext_scancode) {
	//	scancode = ((0xe00) | scancode);
	//	ext_scancode = false;
	//}

	//break_code = ((scancode & 0x0080) != 0);
	//if (break_code) {
	//	uint16_t make_code = (scancode &= 0xff7f);
	//	if (make_code == ctrl_l_make || make_code == ctrl_r_make) {
	//		ctrl_status = false;
	//	} else if (make_code == shift_l_make || make_code == shift_r_make) {
	//		shift_status = false;
	//	} else if (make_code == alt_l_make || make_code == alt_r_make) {
	//		alt_status = false;
	//	} // 
	//	return;
	//} else if ((scancode > 0x00 && scancode < 0x3b) || \
	//			(scancode == alt_r_make) || \
	//			(scancode == ctrl_r_make)) {
	//	bool shift = false;
	//	if ((scancode < 0x0e) || (scancode == 0x29) || \
	//		(scancode == 0x1a) || (scancode == 0x1b) || \
	//		(scancode == 0x2b) || (scancode == 0x27) || \
	//		(scancode == 0x28) || (scancode == 0x33) || \
	//		(scancode == 0x34) || (scancode == 0x35)) {
	//			if (shift_down_last) {
	//				shift = true;
	//			} else {
	//				if (shift_down_last && caps_lock_last) {
	//					shift = false;
	//				} else if (shift_down_last || caps_lock_last) {
	//					shift = true;
	//				} else {
	//					shift = false;
	//				}
	//			}
	//			uint8_t index = (scancode &= 0x00ff);
	//			char cur_char = keymap[index][shift];
	//			if (cur_char) {
	//				if (!ioqueue_full(&kbd_buf)) {
	//					kputchar(cur_char);
	//					ioqueue_putchar(&kbd_buf, cur_char);
	//				}
	//				return;
	//			}
	//			if (scancode == ctrl_l_make || scancode == ctrl_r_make) {
	//				ctrl_status = true;
	//			} else if (scancode == shift_l_make || scancode == shift_r_make) {
	//				shift_status = true;
	//			} else if (scancode == caps_lock_make) {
	//				caps_lock_status = !caps_lock_status;
	//			}
	//	} else {
	//		kprintf("unknown key\n");
	//	}
	//} 
} 

void keyboard_init()
{
	kprintf("keyboard init start\n");
	//ioqueue_init(&kbd_buf);
	kprintf("keyboard init done\n");
}
