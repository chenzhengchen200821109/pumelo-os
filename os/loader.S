#include "boot.h"

.section .text
.global loader
loader:
.code16

    movw $mesg, %ax
    movw %ax, %bp
    movw $0x8, %cx
    movw $0x1301, %ax
    movw $0x2, %bx
    int $0x10

spin:
    jmp spin

mesg:
    .ascii "2 Loader"
