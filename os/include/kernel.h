#ifndef _KERNEL_H
#define _KERNEL_H

#define RPL0 0
#define RPL3 3

#define GDT_TI 0

// GDT for kernel
#define SELECTOR_KERNEL_CODE ((0x1 << 3) + (GDT_TI << 2) + RPL0)
#define SELECTOR_KERNEL_DATA ((0x2 << 3) + (GDT_TI << 2) + RPL0)
#define SELECTOR_KERNEL_STACK SELECTOR_KERNEL_DATA
#define SELECTOR_KERNEL_GS ((0x3 << 3) + (GDT_TI << 2) + RPL0)

/*------------------ IDT descriptor attributes ------------------*/
#define IDT_DESC_P 1
#define IDT_DESC_DPL0 0
#define IDT_DESC_DPL3 3
#define IDT_DESC_32_TYPE 0xe

#define IDT_DESC_ATTR_DPL0 ((IDT_DESC_P << 7) + (IDT_DESC_DPL0 << 5) + IDT_DESC_32_TYPE)
#define IDT_DESC_ATTR_DPL3 ((IDT_DESC_P << 7) + (IDT_DESC_DPL3 << 5) + IDT_DESC_32_TYPE)

#endif
