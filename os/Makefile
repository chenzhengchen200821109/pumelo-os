
AS = as
LD = ld
OBJCOPY = objcopy
DD = dd
CC = gcc

LDFLAGS = -N -e start -Ttext

INCLUDE = -I include/
KERNELDIR = kernel/

HDIMG = hd60M.img

LOADER = loader
MBR = mbr
KERNEL = main

all: $(HDIMG) $(LOADER) $(MBR) $(KINCLUDE)$(KERNEL)

$(MBR): mbr.S
	$(AS) $(INCLUDE) -o $(MBR).o mbr.S
	$(LD) $(LDFLAGS) 0x7c00 -s -o $(MBR) $(MBR).o
	$(OBJCOPY) -S -O binary $(MBR)
	$(DD) if=$(MBR) of=$(HDIMG) bs=512 count=1 seek=0 conv=notrunc


$(LOADER): loader.S helper.S
	$(AS) $(INCLUDE) -o $(LOADER).o loader.S
	$(AS) -o helper.o helper.S
	$(LD) -N -e loader -Ttext 0x900 -Tdata 0xd00 -s -o $(LOADER) $(LOADER).o helper.o
	$(OBJCOPY) -S -O binary $(LOADER)
	$(DD) if=$(LOADER) of=$(HDIMG) bs=512 count=8 seek=2 conv=notrunc

$(HDIMG):
	bximage -hd=60 -mode=create -imgmode=flat -q $(HDIMG)

$(KERNEL): $(KERNELDIR)main.c
	$(CC) -c -nostdlib -static -fno-asynchronous-unwind-tables -o $(KERNELDIR)$(KERNEL).o $(KERNELDIR)main.c
	$(LD) -e main -Ttext 0xc0001500 -s -o $(KERNELDIR)$(KERNEL) $(KERNELDIR)$(KERNEL).o
	$(DD) if=$(KERNELDIR)$(KERNEL) of=$(HDIMG) bs=512 count=127 seek=10 conv=notrunc

clean:
	rm hd60M.img loader mbr loader.o mbr.o $(KERNELDIR)$(KERNEL) $(KERNELDIR)$(KERNEL).o helper.o
