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
.equ DESC_CODE_WORD1, 0x0000 
.equ DESC_CODE_WORD2, 0x9800 
.equ DESC_CODE_WORD3, 0x00cf

/* data segment descriptor */
.equ DESC_DATA_WORD0, 0xffff
.equ DESC_DATA_WORD1, 0x0000
.equ DESC_DATA_WORD2, 0x9200
.equ DESC_DATA_WORD3, 0x00cf

/* video data segment descriptor */
.equ DESC_VIDEO_WORD0, 0x0007
.equ DESC_VIDEO_WORD1, 0x8000
.equ DESC_VIDEO_WORD2, 0x920b
.equ DESC_VIDEO_WORD3, 0x00c0

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
    movw $0x14, %cx
    movw $0x1301, %ax
    movw $0x02, %bx
    movw $0x100, %dx
    int $0x10

/* 
 * set new cursor position, string "kernel" will be 
 * printed out at this position 
 */
    movb $0x2, %ah
    movb $0x0, %bh
    movw $0x0400, %dx
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

/* now we are in protector mode */
.code32
p_mode_start:
    movw $SELECTOR_DATA, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movl $LOADER_STACK_TOP, %esp /* %esp = 0x900 */
    movw $SELECTOR_VIDEO, %ax
    movw %ax, %gs

    /* manipulate graphic memory directly */
    movl $0x1a, %ecx
    movl $0xb8000, %edi
    movl $320, %ebx
    movl $0x0, %esi
display0:
    movb promesg(, %esi, 0x1), %al
    movb $0x2, %ah
    movw %ax, (%edi, %ebx, 0x1)
    addl $0x2, %ebx
    addl $0x1, %esi
    loop display0

    /* setup page table */
    call setup_page
    sgdt GDT_DESC

    /* video data segment */
    movl $0x1, %edi
    movl GDT_DESC(, %edi, 0x2), %ebx

    movl $0x1c, %esi
    orl $KERNEL_START_VA, (%ebx, %esi, 0x1)

    addl $KERNEL_START_VA, GDT_DESC(, %edi, 0x2)
    addl $KERNEL_START_VA, %esp

    /* page mode enable */
    movl $PAGE_DIR_TABLE_ADDR, %eax
    movl %eax, %cr3

    movl %cr0, %eax
    orl $CR0_PG_ON, %eax
    movl %eax, %cr0

    lgdt GDT_DESC

    /* manipulate graphic memory directly */
    movl $0x1b, %ecx
    movl $0xc00b8000, %edi
    movl $480, %ebx
    movl $0x0, %esi
display1:
    movb virmesg(, %esi, 0x1), %al
    movb $0x2, %ah
    movw %ax, (%edi, %ebx, 0x1)
    addl $0x2, %ebx
    addl $0x1, %esi
    loop display1

    jmp $SELECTOR_CODE, $enter_kernel

enter_kernel:

    call kernel_init
    movl $0xc009f000, %esp
    jmp KERNEL_ENTRY_POINT    /* 0xc0001500 */

spin:
    jmp spin

setup_page:
/* setup page directory */
    movl $0x400, %ecx
    movl $0x0, %esi
    movl $PAGE_DIR_TABLE_ADDR, %eax /* 0x100000 */
.clear_page_dir:
    movl $0x0, (%eax, %esi, 0x1)
    addl $0x4, %esi
    loop .clear_page_dir

.create_pde:
    movl $PAGE_DIR_TABLE_ADDR, %eax
    addl $0x1000, %eax  /* %eax store the first page table address */
    movl %eax, %ebx     /* %ebx store the first page table address */
    orl $(PG_US_U | PG_RW_W | PG_P), %eax
    /* 0th page directory entry */
    movl $PAGE_DIR_TABLE_ADDR, %ecx
    movl %eax, (%ecx)

    /* 768th page directory entry */
    movl $768, %edi
    movl %eax, (%ecx, %edi, 0x4)

    /* 1023th page directory entry */
    subl $0x1000, %eax  /* %eax store page direcotry table address */
    movl $4092, %edi
    movl %eax, (%ecx, %edi, 0x1)

    /* PTE */
    movl $256, %ecx
    movl $0x0, %esi
    movl $0x0, %edx
    movl $(PG_US_U | PG_RW_R | PG_P), %edx
.create_pte:
    movl %edx, (%ebx, %esi, 0x4)
    addl $0x1000, %edx
    incl %esi
    loop .create_pte
/* kernel PDE */
    movl $PAGE_DIR_TABLE_ADDR, %eax
    addl $0x2000, %eax
    orl $(PG_US_U | PG_RW_W | PG_P), %eax
    movl $PAGE_DIR_TABLE_ADDR, %ebx
    movl $254, %ecx
    movl $769, %esi
.create_kernel_pde:
    movl %eax, (%ebx, %esi, 0x4)
    incl %esi
    addl $0x1000, %eax
    loop .create_kernel_pde
    ret

/* 
 * load each program segment at the specified memory location 
 * according to ELF format 
 */
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

each_segment:
    cmpl $PTNULL, (%ebx)
    je .PT_NULL
    pushl 16(%ebx)                       /* p_filesz - cnt */
    movl 4(%ebx), %eax                   /* p_offset */
    addl $KERNEL_BASE_ADDR, %eax         /* 0x70000 + 0x500 - src */
    pushl %eax
    pushl 8(%ebx)                        /* p_vaddr - dst */ 

    call mem_cpy
    addl $12, %esp
    addl %edx, %ebx
    loop each_segment
.PT_NULL:
    ret

.section .data                           /* 0xc0000d00 */
.global TOTAL_MEM_BYTES 
.global GDT_DESC
TOTAL_MEM_BYTES:                         /* 0xc0000d00 */
    .long 0
    .long 0
GDT_BASE:                                /* 0xc0000d08 */
    .word 0, 0                         
    .byte 0, 0, 0, 0
CODE_DESC:                               /* 0xc0000d10 */
    .word DESC_CODE_WORD0                
    .word DESC_CODE_WORD1
    .word DESC_CODE_WORD2
    .word DESC_CODE_WORD3
DATA_STACK_DESC:                         /* 0xc0000d18 */    
    .word DESC_DATA_WORD0
    .word DESC_DATA_WORD1
    .word DESC_DATA_WORD2
    .word DESC_DATA_WORD3
VIDEO_DESC:                              /* 0xc0000d20 */
    .word DESC_VIDEO_WORD0
    .word DESC_VIDEO_WORD1
    .word DESC_VIDEO_WORD2
    .word DESC_VIDEO_WORD3
TSS_DESC:                                /* 0xc0000d28 */
    .word 0
    .word 0
    .word 0
    .word 0
USER_CODE_DESC:                          /* 0xc0000d30 */
    .word 0
    .word 0
    .word 0
    .word 0
USER_DATA_DESC:                          /* 0xc0000d38 */
    .word 0
    .word 0
    .word 0
    .word 0
GDT_END:                                 /* 0xc0000d40 */
    .equ GDT_SIZE, (GDT_END - GDT_BASE)
    .equ GDT_LIMIT, (GDT_SIZE - 0x1)
GDT_DESC:                                /* 0xc0000d48 */
    .word GDT_LIMIT
    .long GDT_BASE
ARDS_BUF:
    .rept 100
    .long 0
    .endr
ARDS_NUM:
    .word 0
loadermesg:
    .ascii "Loader is booting..."
promesg:
    .ascii "Entering protected mode..."
virmesg:
    .ascii "Enable virtual page mode..."

