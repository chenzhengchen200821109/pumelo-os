# interrupt handlers
.section .text
.globl kputs
.globl vector0
vector0:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector1
vector1:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector2
vector2:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector3
vector3:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector4
vector4:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector5
vector5:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector6
vector6:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector7
vector7:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector8
vector8:
    nop
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector9
vector9:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector10
vector10:
    nop
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector11
vector11:
    nop
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector12
vector12:
    nop
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector13
vector13:
    nop
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector14
vector14:
    nop
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector15
vector15:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector16
vector16:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector17
vector17:
    nop
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector18
vector18:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector19
vector19:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector20
vector20:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector21
vector21:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector22
vector22:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector23
vector23:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector24
vector24:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector25
vector25:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector26
vector26:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector27
vector27:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector28
vector28:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector29
vector29:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector30
vector30:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.globl vector31
vector31:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret
.global vector32
vector32:
    pushl $0
    pushl $intr_str
    call kputs
    addl $4, %esp
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    addl $4, %esp
    iret

# interrupt vector table
.section .data
.globl intr_entry_table
intr_entry_table:
    .long vector0
    .long vector1
    .long vector2
    .long vector3
    .long vector4
    .long vector5
    .long vector6
    .long vector7
    .long vector8
    .long vector9
    .long vector10
    .long vector11
    .long vector12
    .long vector13
    .long vector14
    .long vector15
    .long vector16
    .long vector17
    .long vector18
    .long vector19
    .long vector20
    .long vector21
    .long vector22
    .long vector23
    .long vector24
    .long vector25
    .long vector26
    .long vector27
    .long vector28
    .long vector29
    .long vector30
    .long vector31
    .long vector32
intr_str:
    .ascii "interrupt occur"
