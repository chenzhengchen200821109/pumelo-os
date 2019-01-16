# vectors.S sends all traps here.
.equ SELECTOR_KERNEL_DATA, 0x10

.globl trap_handler
.section .text
.globl __alltraps
__alltraps:
    # push registers to build a trap frame
    # therefore make the stack look like a struct trapframe
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    pushal

    # load kernel data selector into %ds and %es to set up data segments for kernel
    movl $SELECTOR_KERNEL_DATA, %eax
    movw %ax, %ds
    movw %ax, %es

    call trap_handler
    jmp __trapret

# return falls through to trapret...
.section .text
.globl __trapret
__trapret:
    # restore registers from stack
    popal

    # restore %ds, %es, %fs and %gs
    popl %gs
    popl %fs
    popl %es
    popl %ds

    # get rid of the trap number and error code
    addl $0x8, %esp
    iret

