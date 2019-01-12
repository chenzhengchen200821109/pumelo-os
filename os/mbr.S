# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0, %ip=7c00.

# start address should be 0:7c00, in real mode, the beginning address of the 
# running mbr.
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
    movb $0x3, %ah
    movb $0x0, %bh
    int $0x10

    # print out a string
    movw $message, %ax
    movw %ax, %bp
    movw $0x5, %cx
    movw $0x1301, %ax
    movw $0x2, %bx
    int $0x10

    movl $LOADER_START_SECTOR, %eax
    movw $LOADER_BASE_ADDR, %bx
    movw $0x8, %cx
    call rd_disk_m_16

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

    movw %di, %ax       /* */
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
    .ascii "1 MBR"
    .org 510, 0
    .word 0xaa55
