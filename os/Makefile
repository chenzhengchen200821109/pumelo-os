
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

$(KERNEL): $(KERNELDIR)main.c $(KERNELDIR)console.c $(KERNELDIR)string.c $(KERNELDIR)stdio.c $(KERNELDIR)printfmt.c $(KERNELDIR)vector.S $(KERNELDIR)intr.c $(KERNELDIR)trapentry.S $(KERNELDIR)trap.c $(KERNELDIR)clock.c $(KERNELDIR)picirq.c $(KERNELDIR)vector.S $(KERNELDIR)trapentry.S $(KERNELDIR)panic.c $(KERNELDIR)pmm.c $(KERNELDIR)mem.c $(DSDIR)bitmap.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o $(KERNEL).o $(KERNELDIR)main.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o console.o $(KERNELDIR)console.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o string.o $(KERNELDIR)string.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o stdio.o $(KERNELDIR)stdio.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o printfmt.o $(KERNELDIR)printfmt.c
	$(CC) $(INCLUDE) -c -nostdlib -static -fno-asynchronous-unwind-tables -o intr.o $(KERNELDIR)intr.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o trap.o $(KERNELDIR)trap.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o clock.o $(KERNELDIR)clock.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o picirq.o $(KERNELDIR)picirq.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o panic.o $(KERNELDIR)panic.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o pmm.o $(KERNELDIR)pmm.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o mem.o $(KERNELDIR)mem.c
	$(CC) $(INCLUDE) $(CCFLAGS) -o bitmap.o $(DSDIR)bitmap.c
	$(AS) $(INCLUDE) -o vector.o $(KERNELDIR)vector.S
	$(AS) $(INCLUDE) -o trapentry.o $(KERNELDIR)trapentry.S
	$(LD) -e main -Ttext 0xc0001500 -o $(KERNEL) $(KERNEL).o console.o string.o stdio.o printfmt.o intr.o trap.o clock.o picirq.o panic.o pmm.o mem.o bitmap.o vector.o trapentry.o
	$(DD) if=$(KERNEL) of=$(HDIMG) bs=512 count=127 seek=10 conv=notrunc

clean:
	rm hd60M.img loader mbr loader.o mbr.o $(KERNEL) $(KERNEL).o helper.o intr.o trap.o clock.o picirq.o panic.o pmm.o mem.o bitmap.o vector.o trapentry.o
