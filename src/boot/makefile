# EPOS Boot Makefile

include ../../makedefs

TARGET := $(if $(shell find $(MMOD)* 2> /dev/null), $(MMOD)_install)
ELF_HEADER_OFFSET := sh -c "$(OBJDUMP) -p $(IMG)/$(MMOD)_setup | sed -n -e 's/^ *LOAD off *\(0x.*\) vaddr.*$$/\1/ p' | head -1"

all: 		$(TARGET)

legacy_pc_boot: legacy_pc_boot.o
		ld86 -0 -s -T $(BOOT_ADDR) -o $@ $<

legacy_pc_boot.o: legacy_pc_boot.s
		sed -e 's/^ELF_HDR_SIZE =.*$$/ELF_HDR_SIZE = $(shell $(ELF_HEADER_OFFSET))/' -i $<
		as86 -0 -o $@ $<

legacy_pc_install: legacy_pc_boot
		$(DD) if=$< of=$<.tmp ibs=32 skip=1 obs=512 >& /dev/null
		$(INSTALL) $(MMOD)_boot.tmp $(IMG)/$(MMOD)_boot
		$(CLEAN) $(MMOD)_boot.tmp

clean:
		$(CLEAN) *.s *.o *_boot *.tmp

