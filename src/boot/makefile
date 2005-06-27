# EPOS-- Boot Makefile

include $(EPOS)/makedefs

all: 		install

#PC
pc_boot:        pc_boot.o
		ld86 -0 -s -T $(BOOT_ADDR) -o $@ $<

pc_boot.o:      pc_boot.s
		as86 -0 -o $@ $<

pc_install:	pc_boot
		$(DD) if=$< of=$<.img ibs=32 skip=1 obs=512


#RCX
rcx_boot:	rcx_boot.o
		$(LD) $(LDFLAGS) -L$(CCLIB) -N -Ttext $(BOOT_ADDR) -e $(BOOT_ADDR) \
		-o $@ $< -l$(LUTIL) -l$(LSYS) -l$(LINIT) -lc -lgcc

rcx_install:	rcx_boot
		$(OBJCPY) -O binary $< $<.img


#AVRMCU
avrmcu_boot: FORCE
		$(TOUCH) $(MACH)_boot

avrmcu_install: $(MACH)_boot
		$(TOUCH) $(MACH)_boot.img

#Mica2
mica2_boot: FORCE
		$(TOUCH) $(MACH)_boot

mica2_install: $(MACH)_boot
		$(TOUCH) $(MACH)_boot.img


install:        $(MACH)_install
		$(INSTALL) $(MACH)_boot.img $(IMG)/$(MACH)_boot
		$(CLEAN) $(MACH)_boot.img

test:

clean:
		$(CLEAN) *.s *.o *_boot *.tmp

FORCE:
