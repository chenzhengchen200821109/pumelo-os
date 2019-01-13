.section .text
.type mem_cpy, @function
.global mem_cpy
mem_cpy:
    cld
    push %ebp
    movl %esp, %ebp
    pushl %ecx

    movl 8(%ebp), %edi     /* dst */
    movl 12(%ebp), %esi    /* src */
    movl 16(%ebp), %ecx    /* cnt */
    rep movsb

    popl %ecx
    movl %ebp, %esp
    popl %ebp
    ret
