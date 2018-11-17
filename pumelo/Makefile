EMPTY	:=
SPACE	:= $(EMPTY) $(EMPTY)
SLASH	:= /

GCCPREFIX := /usr/bin

# eliminate default suffix rules
#.SUFFIXES: .c .S .h

# delete target files if there is an error (or make is interrupted)
#.DELETE_ON_ERROR:

# define compiler and flags
CC		:= $(GCCPREFIX)$(SLASH)gcc
CFLAGS	:= -Os -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc -fno-stack-protector -c

CTYPE	:= c S

# define linker and flags
LD      := $(GCCPREFIX)$(SLASH)ld
LDFLAGS	:= -m elf_i386 -nostdlib

OBJCOPY := $(GCCPREFIX)$(SLASH)objcopy
OBJDUMP := $(GCCPREFIX)$(SLASH)objdump

COPY	:= cp
MKDIR   := mkdir -p
MV		:= mv
RM		:= rm -f
AWK		:= awk
SED		:= sed
SH		:= sh
TR		:= tr
TOUCH	:= touch -c

# obj and bin directory
OBJDIR	:= obj
BINDIR	:= bin

ALLOBJS	:=
ALLDEPS	:=
TARGETS	:=

#include tools/function.mk

#listf_cc = $(call listf,$(1),$(CTYPE))

# for cc
#add_files_cc = $(call add_files,$(1),$(CC),$(CFLAGS) $(3),$(2),$(4))
#create_target_cc = $(call create_target,$(1),$(2),$(3),$(CC),$(CFLAGS))

# for hostcc
#add_files_host = $(call add_files,$(1),$(HOSTCC),$(HOSTCFLAGS),$(2),$(3))
#create_target_host = $(call create_target,$(1),$(2),$(3),$(HOSTCC),$(HOSTCFLAGS))

#cgtype = $(patsubst %.$(2),%.$(3),$(1))
#objfile = $(call toobj,$(1))
#asmfile = $(call cgtype,$(call toobj,$(1)),o,asm)
#outfile = $(call cgtype,$(call toobj,$(1)),o,out)
#symfile = $(call cgtype,$(call toobj,$(1)),o,sym)

# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# include kernel/user
BOOT_INCLUDE = -I boot/
LIBS_INCLUDE = -I libs/

#LIBDIR	+= libs

#$(call add_files_cc,$(call listf_cc,$(LIBDIR)),libs,)

# -------------------------------------------------------------------
# kernel

#KINCLUDE	+= kern/debug/ \
			   kern/driver/ \
			   kern/trap/ \
			   kern/mm/

#KSRCDIR		+= kern/init \
			   kern/libs \
			   kern/debug \
			   kern/driver \
			   kern/trap \
			   kern/mm

#KCFLAGS		+= -I $(KINCLUDE)

#$(call add_files_cc,$(call listf_cc,$(KSRCDIR)),kernel,$(KCFLAGS))

#KOBJS = $(KINCLUDE)*.o
#KOBJS += $(INCLUDE)*.o 

# create kernel target
#kernel = kernel

#$(kernel): $(KOBJS)
#	echo + ld $@
#	$(LD) $(LDFLAGS) -T tools/kernel.ld -o $@ $(KOBJS)

# -------------------------------------------------------------------

# create bootblock
#bootfiles = $(call listf_cc,boot)
#$(foreach f,$(bootfiles),$(call cc_compile,$(f),$(CC),$(CFLAGS) -Os -nostdinc))

bootblock = bootblock
bootsrc = boot/bootasm.S boot/bootmain.c
bootobj = bootasm.o bootmain.o

$(bootblock): $(bootsrc) 
	@echo + ld $@
	$(CC) $(CFLAGS) $(LIBS_INCLUDE) $(BOOT_INCLUDE) $(bootsrc)
	$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 $(bootobj) -o $(bootblock)
	$(OBJCOPY) -S -O binary $(bootblock) $(bootblock)

#$(call create_target,bootblock)

# -------------------------------------------------------------------

# create OS image
POSIMG = pos.img

$(POSIMG): $(bootblock)
	dd if=/dev/zero of=$@ bs=512 count=1000
	dd if=$(bootblock) of=$@ bs=512 seek=1 conv=notrunc
	#dd if=$(kernel) of=$@ bs=512 seek=1 conv=notrunc

# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

