# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0, %ip=7c00.

# start address should be 0:7c00, in real mode, the beginning address of the 
# running mbr.
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

    # loop
spin:
    jmp spin

message:
    .ascii "1 MBR"
    .org 510, 0
    #.byte 0x55, 0xaa
    .word 0xaa55
