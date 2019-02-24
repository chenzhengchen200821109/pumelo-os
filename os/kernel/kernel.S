# handler
//.extern kputs
.extern general_intr_handler

//.section .data
//intr_str:
//    .ascii "interrupt occur"
//time_intr_str:
//    .ascii "time interrupt occur"

.section .text
.globl vector0
vector0:
    pushl $0 # tf_err field of struct trapframe
    //push $intr_str
    //call kputs
    //addl $4, %esp
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    pushl $0 # tf_trapno field of struct trapframe
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    jmp intr_exit
    //addl $4, %esp
    //iret
.globl vector1
vector1:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $1
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector2
vector2:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $2
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector3
vector3:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $3
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector4
vector4:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $4
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector5
vector5:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $5
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector6
vector6:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $6
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector7
vector7:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $7
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector8
vector8:
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $8
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector9
vector9:
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $9
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector10
vector10:
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $10
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector11
vector11:
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $11
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector12
vector12:
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $12
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector13
vector13:
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $13
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector14
vector14:
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $14
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector15
vector15:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $15
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector16
vector16:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $16
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector17
vector17:
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $17
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector18
vector18:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $18
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector19
vector19:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $19
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector20
vector20:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $20
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector21
vector21:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $21
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector22
vector22:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $22
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector23
vector23:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $23
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector24
vector24:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $24
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector25
vector25:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $25
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector26
vector26:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $26
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector27
vector27:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $27
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector28
vector28:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $28
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector29
vector29:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $29
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector30
vector30:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $30
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector31
vector31:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $intr_str
    //call kputs
    //addl $4, %esp
    pushl $31
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector32
vector32:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $32
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector33
vector33:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $33
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector34
vector34:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $34
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector35
vector35:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $35
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector36
vector36:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $36
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector37
vector37:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $37
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector38
vector38:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $38
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector39
vector39:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $39
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector40
vector40:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $40
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector41
vector41:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $41
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector42
vector42:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $42
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector43
vector43:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $43
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector44
vector44:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $44
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector45
vector45:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $45
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector46
vector46:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $46
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector47
vector47:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $47
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector48
vector48:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $48
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector49
vector49:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $49
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit
.globl vector50
vector50:
    pushl $0
    push %ds
    push %es
    push %fs
    push %gs
    pushal
    //push $time_intr_str
    //call kputs
    //addl $4, %esp
    pushl $50
    movb $0x20, %al
    outb %al, $0xa0
    outb %al, $0x20
    call general_intr_handler
    //addl $4, %esp
    //iret
    jmp intr_exit

.section .text
.global intr_exit
intr_exit:
    addl $4, %esp
    popal
    pop %gs
    pop %fs
    pop %es
    pop %ds
    addl $4, %esp
    iret

# vector table
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
  .long vector32  # timer interrupt
  .long vector33  # keyboard interrupt
  .long vector34
  .long vector35
  .long vector36
  .long vector37
  .long vector38
  .long vector39
  .long vector40
  .long vector41
  .long vector42
  .long vector43
  .long vector44  # ps/2 mouse
  .long vector45  # FPU float unit
  .long vector46  # disk interrupt
  .long vector47
  .long vector48
  .long vector49
  .long vector50
  
