# eposmkbi Makefile

include	../../makedefs

all		: install

eposmkbi	: eposmkbi.cc
		$(TCXX) $(TCXXFLAGS) $<
		$(TLD) $(TLDFLAGS) -o $@ eposmkbi.o

eposmkbi.conf:	
		@echo "MODE=$(MODE)" >! $@
		@echo "MACH=$(MACH)" >> $@
		@echo "ARCH=$(ARCH)" >> $@
		@echo "CLOCK=$(ARCH_CLOCK)" >> $@
		@echo "WORD_SIZE=$(ARCH_WORD_SIZE)" >> $@
		@echo "ENDIANESS=$(ARCH_ENDIANESS)" >> $@
		@echo "MEM_BASE=$(MEM_BASE)" >> $@
		@echo "MEM_TOP=$(MEM_TOP)" >> $@
		@echo "BOOT_LENGTH_MIN=$(BOOT_LENGTH_MIN)" >> $@
		@echo "BOOT_LENGTH_MAX=$(BOOT_LENGTH_MAX)" >> $@

install		: eposmkbi eposmkbi.conf
		$(INSTALL) eposmkbi $(BIN)
		$(INSTALL) eposmkbi.conf $(ETC)

clean		:
		$(CLEAN) *.o eposmkbi eposmkbi.conf
