#include "boot.h"

/* 
 * define static symbols 
 */
.equ LOADER_STACK_TOP, 0x900
.equ SELECTOR_CODE, 0x8
.equ SELECTOR_DATA, 0x10
.equ SELECTOR_VIDEO, 0x18

/* code segment descriptor */
.equ DESC_CODE_WORD0, 0xffff
.equ DESC_CODE_WORD1, 0x0
.equ DESC_CODE_WORD2, 0x9800 
.equ DESC_CODE_WORD3, 0x00cf

/* data segment descriptor */
.equ DESC_DATA_WORD0, 0xffff
.equ DESC_DATA_WORD1, 0x0
.equ DESC_DATA_WORD2, 0x9200
.equ DESC_DATA_WORD3, 0x00cf

/* video data segment descriptor */
.equ DESC_VIDEO_WORD0, 0x7
.equ DESC_VIDEO_WORD1, 0x8000
.equ DESC_VIDEO_WORD2, 0x920b
.equ DESC_VIDEO_WORD3, 0xc0

.equ CRO_PE_ON, 0x00000001
.equ CR0_PG_ON, 0x80000000

.equ PG_P, 0x1
.equ PG_RW_R, 0x0
.equ PG_RW_W, 0x2
.equ PG_US_S, 0x0
.equ PG_US_U, 0x4

.equ PAGE_DIR_TABLE_ADDR, 0x100000        /* 1MB = 0xfffff */
.equ KERNEL_START_VA, 0xc0000000

.equ KERNEL_BASE_ADDR, 0x70000
.equ PTNULL, 0x0
.equ KERNEL_ENTRY_POINT, 0xc0001500

.section .text
.global loader
.code16
loader:

/* INT 0x15 -- get physical memory capacity */
    xorl %ebx, %ebx
    movl $0x534d4150, %edx
    mov $ARDS_BUF, %di
e820_mem_get_loop:
    movl $0xe820, %eax
    movl $0x14, %ecx
    int $0x15
    jc e820_failed                /* jmp if CF=1 */
    addw %cx, %di
    incw ARDS_NUM
    cmpl $0x0, %ebx
    jnz e820_mem_get_loop

    movw ARDS_NUM, %cx
    movl $ARDS_BUF, %ebx
    xorl %edx, %edx

find_max_mem_area:
    movl (%ebx), %eax
    add 8(%ebx), %eax
    addl $0x14, %ebx
    cmpl %eax, %edx
    jge next_ards
    movl %eax, %edx
next_ards:
    loop find_max_mem_area
    jmp mem_get_ok

e820_failed:
    cli                              /* disable interrupts */
    hlt
mem_get_ok:
    movl %edx, TOTAL_MEM_BYTES

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

    /* */
    call setup_page
    sgdt GDT_DESC

    /* video data segment */
    movl $0x1, %edi
    movl GDT_DESC(, %edi, 0x2), %ebx

    movl $0x1c, %esi
    orl $KERNEL_START_VA, (%ebx, %esi, 0x1)

    addl $KERNEL_START_VA, GDT_DESC(, %edi, 0x2)
    addl $KERNEL_START_VA, %esp

    movl $PAGE_DIR_TABLE_ADDR, %eax
    movl %eax, %cr3

    movl %cr0, %eax
    orl $CR0_PG_ON, %eax
    movl %eax, %cr0

    lgdt GDT_DESC

    movb $'V', %gs:160
    movb $0xa4, %gs:161

    jmp $SELECTOR_CODE, $enter_kernel

enter_kernel:

    call kernel_init
    movl $0xc009f000, %esp
    jmp KERNEL_ENTRY_POINT

spin:
    jmp spin

setup_page:
/* setup page directory */
    movl $0x400, %ecx
    movl $0x0, %esi
    movl $PAGE_DIR_TABLE_ADDR, %eax
.clear_page_dir:
    movl $0x0, (%eax, %esi, 0x1)
    addl $0x4, %esi
    loop .clear_page_dir
.create_pde:
    movl $PAGE_DIR_TABLE_ADDR, %eax
    addl $0x1000, %eax  /* %eax store the first page table address */
    movl %eax, %ebx
    orl $(PG_US_U | PG_RW_W | PG_P), %eax
    movl $PAGE_DIR_TABLE_ADDR, %ecx
    movl %eax, (%ecx)
    #movl $0xc00, %edi
    movl $768, %edi
    movl %eax, (%ecx, %edi, 0x4)

    subl $0x1000, %eax  /* %eax store the last page direcotry entry */
    movl $4092, %edi
    movl %eax, (%ecx, %edi, 0x1)

/* PTE */
    movl $256, %ecx
    movl $0x0, %esi
    movl $(PG_US_U | PG_RW_R | PG_P), %edx
.create_pte:
    movl %edx, (%ebx, %esi, 0x4)
    addl $0x1000, %edx
    incl %esi
    loop .create_pte
/* kernel PDE */
    movl $PAGE_DIR_TABLE_ADDR, %eax
    addl 0x2000, %eax
    orl $(PG_US_U | PG_RW_W | PG_P), %eax
    movl $PAGE_DIR_TABLE_ADDR, %ebx
    movl $254, %ecx
    movl $769, %esi
.create_kernel_pde:
    movl %eax, (%ebx, %esi, 0x4)
    incl %esi
    addl 0x1000, %eax
    loop .create_kernel_pde
    ret

.type kernel_init, @function
kernel_init:
    xorl %eax, %eax
    xorl %ebx, %ebx
    xorl %ecx, %ecx
    xorl %edx, %edx

    //movl (KERNEL_BASE_ADDR + 24), %eax   /* e_entry */
    movw (KERNEL_BASE_ADDR + 42), %dx    /* e_phentsize */
    movl (KERNEL_BASE_ADDR + 28), %ebx   /* e_phoff */
    addl $KERNEL_BASE_ADDR, %ebx         /* program header table */
    movw (KERNEL_BASE_ADDR + 44), %cx   /* e_phnum */
    subw $0x1, %cx

.each_segment:
    cmpl $PTNULL, (%ebx)
    je .PT_NULL
    pushl 16(%ebx)                       /* p_filesz - cnt */
    //subl 8(%ebx), %eax
    //addl 4(%ebx), %eax                   /* p_offset */
    //addl $KERNEL_BASE_ADDR, %eax         /* 0x70000 + 0x500 - src */
    movl $KERNEL_BASE_ADDR, %eax
    pushl %eax
    pushl 8(%ebx)                        /* p_vaddr - dst */ 

    call mem_cpy
    addl $12, %esp
.PT_NULL:
    addl %edx, %ebx
    loop .each_segment
    ret

.section .data
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
.global TOTAL_MEM_BYTES
TOTAL_MEM_BYTES:
    .long 0
ARDS_BUF:
    .rept 100
    .long 0
    .endr
ARDS_NUM:
    .word 0
loadermesg:
    .ascii "2 Loader in real."

