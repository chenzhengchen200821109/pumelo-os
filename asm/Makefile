

AS = as
LD = ld
OBJCOPY = objcopy
DD = dd

LDFLAGS = -N -e start -Ttext

HDIMG = hd60M.img


LOADER = loader
MBR = mbr

$(MBR): mbr.S
	$(AS) -o $(MBR).o mbr.S
	$(LD) $(LDFLAGS) 0x7c00 -s -o $(MBR) $(MBR).o
	$(OBJCOPY) -S -O binary $(MBR)
	$(DD) if=$(MBR) of=$(HDIMG) bs=512 count=1 seek=0 conv=notrunc


$(LOADER): loader.S
	$(AS) -o $(LOADER).o loader.S
	$(LD) -N -e loader -Ttext 0x900 -s -o $(LOADER) $(LOADER).o
	$(OBJCOPY) -S -O binary $(LOADER)
	$(DD) if=$(LOADER) of=$(HDIMG) bs=512 count=1 seek=2 conv=notrunc

$(HDIMG):
	bximage -hd=60 -mode=create -imgmode=flat -q $(HDIMG)

