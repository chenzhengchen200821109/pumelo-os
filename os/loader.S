#include "boot.h"

.equ LOADER_STACK_TOP, 0x900
.equ SELECTOR_CODE, 0x8
.equ SELECTOR_DATA, 0x10
.equ SELECTOR_VIDEO, 0x18


.equ DESC_CODE_WORD0, 0xffff
.equ DESC_CODE_WORD1, 0x0
.equ DESC_CODE_WORD2, 0x9800 
.equ DESC_CODE_WORD3, 0x00cf

.equ DESC_DATA_WORD0, 0xffff
.equ DESC_DATA_WORD1, 0x0
.equ DESC_DATA_WORD2, 0x9200
.equ DESC_DATA_WORD3, 0x00cf

.equ DESC_VIDEO_WORD0, 0x7
.equ DESC_VIDEO_WORD1, 0x8000
.equ DESC_VIDEO_WORD2, 0x920b
.equ DESC_VIDEO_WORD3, 0xc0

.equ CRO_PE_ON, 0x1

.section .text
.global loader
.code16
loader:

/* INT 0x10 -- print out a string */
    movw $LOADER_STACK_TOP, %sp
    movw $loadermesg, %bp
    movw $0x11, %cx
    movw $0x1301, %ax
    movw $0x001f, %bx
    movw $0x1800, %dx
    int $0x10

/* enable A20 gate */
    inb $0x92, %al
    orb $0x2, %al
    outb %al, $0x92

/* should we disable interrupt before switching to protector mode */
    #cli

/* load gdt descriptor */
    lgdt GDT_DESC

/* enable PE flag */
    movl %cr0, %eax
    orl $CRO_PE_ON, %eax
    movl %eax, %cr0

    ljmp $SELECTOR_CODE, $p_mode_start

.code32
p_mode_start:
    movw $SELECTOR_DATA, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movl $LOADER_STACK_TOP, %esp
    movw $SELECTOR_VIDEO, %ax
    movw %ax, %gs

    /* manipulate graphic memory directly */
    movb $'P', %gs:160

spin:
    jmp spin

loadermesg:
    .ascii "2 Loader in real."

.p2align 2
GDT_BASE:
    .word 0, 0
    .byte 0, 0, 0, 0
CODE_DESC:
    .word DESC_CODE_WORD0
    .word DESC_CODE_WORD1
    .word DESC_CODE_WORD2
    .word DESC_CODE_WORD3
DATA_STACK_DESC:
    .word DESC_DATA_WORD0
    .word DESC_DATA_WORD1
    .word DESC_DATA_WORD2
    .word DESC_DATA_WORD3
VIDEO_DESC:
    .word DESC_VIDEO_WORD0
    .word DESC_VIDEO_WORD1
    .word DESC_VIDEO_WORD2
    .word DESC_VIDEO_WORD3
GDT_END:
    .equ GDT_SIZE, (GDT_END - GDT_BASE)
    .equ GDT_LIMIT, (GDT_SIZE - 0x1)
GDT_DESC:
    .word GDT_LIMIT
    .long GDT_BASE


