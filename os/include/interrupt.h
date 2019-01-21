#ifndef _INTR_H
#define _INTR_H

#include "defs.h"

typedef uintptr_t intr_handler;

void idt_init();

void register_handler(uint8_t vecno, intr_handler function);

#endif
