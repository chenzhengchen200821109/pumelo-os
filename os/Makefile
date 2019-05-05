###############################################################
AS = as
LD = ld
OBJCOPY = objcopy
DD = dd
CC = gcc

LDFLAGS = -N -e start -Ttext
CCFLAGS = -c -nostdlib -static -fno-asynchronous-unwind-tables 

INCLUDE = -I include/
KERNELDIR = kernel/
BOOTDIR = boot/
DSDIR = ds/

HDIMG = hd60M.img
HD80IMG = hd80M.img

LOADER = loader
MBR = mbr
KERNEL = main

all: $(HDIMG) $(LOADER) $(MBR) $(KINCLUDE)$(KERNEL)

$(MBR): $(BOOTDIR)mbr.S
	$(AS) $(INCLUDE) -o $(MBR).o $(BOOTDIR)mbr.S
	$(LD) $(LDFLAGS) 0x7c00 -s -o $(MBR) $(MBR).o
	$(OBJCOPY) -S -O binary $(MBR)
	$(DD) if=$(MBR) of=$(HDIMG) bs=512 count=1 seek=0 conv=notrunc


$(LOADER): $(BOOTDIR)loader.S $(BOOTDIR)helper.S
	$(AS) $(INCLUDE) -o $(LOADER).o $(BOOTDIR)loader.S
	$(AS) -o helper.o $(BOOTDIR)helper.S
	$(LD) -N -e loader -Ttext 0x900 -Tdata 0xd00 -s -o $(LOADER) $(LOADER).o helper.o
	$(OBJCOPY) -S -O binary $(LOADER)
	$(DD) if=$(LOADER) of=$(HDIMG) bs=512 count=8 seek=2 conv=notrunc

$(HDIMG):
	bximage -hd=60 -mode=create -imgmode=flat -q $(HDIMG)

$(HD80IMG):
	bximage -hd=80 -mode=create -imgmode=flat -q $(HD80IMG)

$(KERNEL): $(KERNELDIR)main.c $(KERNELDIR)console.c $(KERNELDIR)string.c $(KERNELDIR)stdio.c $(KERNELDIR)printfmt.c $(KERNELDIR)interrupt.c $(KERNELDIR)panic.c $(KERNELDIR)pmm.c $(KERNELDIR)mem.c $(DSDIR)bitmap.c $(KERNELDIR)kernel.S $(KERNELDIR)intr.c $(KERNELDIR)clock.c $(DSDIR)list.c $(KERNELDIR)thread.c $(KERNELDIR)switch.S $(KERNELDIR)spinlock.c $(KERNELDIR)sync.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o $(KERNEL).o $(KERNELDIR)main.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o console.o $(KERNELDIR)console.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o string.o $(KERNELDIR)string.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o stdio.o $(KERNELDIR)stdio.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o printfmt.o $(KERNELDIR)printfmt.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o intr.o $(KERNELDIR)intr.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o interrupt.o $(KERNELDIR)interrupt.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o clock.o $(KERNELDIR)clock.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o panic.o $(KERNELDIR)panic.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o pmm.o $(KERNELDIR)pmm.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o mem.o $(KERNELDIR)mem.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o bitmap.o $(DSDIR)bitmap.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o list.o $(DSDIR)list.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o thread.o $(KERNELDIR)thread.c
	$(AS) $(INCLUDE) -o switch.o $(KERNELDIR)switch.S
	$(AS) $(INCLUDE) -o kernel.o $(KERNELDIR)kernel.S
	$(CC) $(INCLUDE) $(CCFLAGS) -o spinlock.o $(KERNELDIR)spinlock.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o lock.o $(KERNELDIR)sync.c
	$(LD) -e main -Ttext 0xc0001500 -o $(KERNEL) $(KERNEL).o console.o string.o stdio.o printfmt.o interrupt.o panic.o pmm.o mem.o bitmap.o kernel.o intr.o clock.o thread.o list.o switch.o spinlock.o lock.o
	$(DD) if=$(KERNEL) of=$(HDIMG) bs=512 count=127 seek=10 conv=notrunc

clean:
	rm hd60M.img loader mbr loader.o mbr.o $(KERNEL) $(KERNEL).o helper.o console.o intr.o clock.o panic.o pmm.o mem.o bitmap.o list.o thread.o interrupt.o switch.o kernel.o stdio.o printfmt.o string.o spinlock.o lock.o 
