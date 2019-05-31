# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0, %ip=7c00.

# start address should be 0:7c00, in real mode, the beginning address of the 
# running mbr.
#
#                    Memory Layout in Real Mode (1MB)
#  ------------------------------------------------------
#  start    end    size            purpose             
#  ------------------------------------------------------
#  F0000   FFFFF   64KB    BIOS code 
#  C8000   EFFFF   160KB   I/O
#  A0000   C7FFF   160KB   Vedio memory
#  9FC00   9FFFF   1KB     EBDA(Extended BIOS Data Area)
#  7E00    9FBFF   608KB   usable memory
#  7C00    7DFF    512B    MBR 
#  500     7BFF    30KB    usable memory
#  400     4FF     256B    BIOS data area
#  000     3FF     1KB     Interrupt Vector Table(IVT)
#  ------------------------------------------------------

.include "boot.h"

.section .text
.global start
start:
.code16    # Assemble for 16-bit mode

    # Set up the important data segment registers (DS, ES, SS, FS).
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %ss
    movw %ax, %fs
    movw $0x7c00, %sp

    # scroll up screen
    movw $0x600, %ax
    movw $0x700, %bx
    movw $0x0, %cx
    movw $0x184f, %dx
    int $0x10

    # get cursor current position
    #movb $0x3, %ah
    #movb $0x0, %bh
    #int $0x10

    # set new cursor position
    movb $0x2, %ah
    movb $0x0, %bh
    movw $0x0, %dx
    int $0x10

    # print out a string
    movw $message, %ax
    movw %ax, %bp
    movw $0x11, %cx
    movw $0x1301, %ax
    movw $0x2, %bx    /* black background and green characters */
    int $0x10

    /* load loader */
    movl $LOADER_START_SECTOR, %eax   /* loader is at sector number 0x2 */
    movw $LOADER_BASE_ADDR, %bx       /* loader base address is 0x900 */
    movw $0x8, %cx
    call rd_disk_m_16

    /* load OS kernel */
    movl $KERNEL_START_SECTOR, %eax   /* kernel is at sector number 0xa */
    movw $0x7000, %bx
    movw %bx, %ds
    movw $0x0, %bx
    movw $127, %cx                    /* number of sectors to read */
    call rd_disk_m_16

    movw $0x0, %bx
    movw %bx, %ds
    jmp LOADER_BASE_ADDR

# rd_disk_m_16
rd_disk_m_16:

    movl %eax, %esi
    movw %cx, %di

    movw $0x1f2, %dx
    movb %cl, %al
    outb %al, %dx

    movl %esi, %eax

    movw $0x1f3, %dx     # 0x1f3 7~0
    outb %al, %dx

    movb $8, %cl         # 0x1f4 15~8
    shr %cl, %eax
    movw $0x1f4, %dx
    out %al, %dx

    shr %cl, %eax        # 0x1f5 23~16
    movw $0x1f5, %dx
    out %al, %dx

    shr %cl, %eax        # 0x1f6
    andb $0xf, %al
    orb $0xe0, %al
    movw $0x1f6, %dx
    out %al, %dx

    movw $0x1f7, %dx
    movb $0x20, %al
    out %al, %dx

not_ready:
    nop
    nop
    inb %dx, %al
    andb $0x88, %al
    cmpb $0x08, %al
    jnz not_ready

    movw %di, %ax       /* %di = 128 */
    movw $256, %dx
    mul %dx
    movw %ax, %cx
    movw $0x1f0, %dx

go_on_read:
    inw %dx, %ax
    movw %ax, (%bx)
    add $2, %bx
    loop go_on_read
    ret

message:
    .ascii "MBR is booting..."
    .equ KERNEL_START_SECTOR, 0xa
    .equ KERNEL_BASE_ADDR, 0x70000
    .org 510, 0
    .word 0xaa55
