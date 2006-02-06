# EPOS-- Boot Makefile

include $(EPOS)/makedefs

all: 		$(MACH)_install

pc_boot:        pc_boot.o
		ld86 -0 -s -T $(BOOT_ADDR) -o $@ $<

pc_boot.o:      pc_boot.s
		as86 -0 -o $@ $<

pc_install:	pc_boot
		$(DD) if=$< of=$<.tmp ibs=32 skip=1 obs=512
		$(INSTALL) $(MACH)_boot.tmp $(IMG)/$(MACH)_boot
		$(CLEAN) $(MACH)_boot.tmp


atmega16_install:
atmega128_install:


test:

clean:
		$(CLEAN) *.s *.o *_boot *.tmp

