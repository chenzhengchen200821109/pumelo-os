#ifndef KEYBOARD_H
#define KEYBOARD_H

extern struct ioqueue kbd_buf;

void keyboard_intr_handler();
void keyboard_init();

#endif
