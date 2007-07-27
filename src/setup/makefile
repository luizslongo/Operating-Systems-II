# EPOS-- Setup Makefile

include ../../makedefs

TARGET := $(if $(shell find $(MACH)*), install)


all:		$(TARGET)

$(MACH)_setup:	$(MACH)_setup.o
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start $(MACH_CODE_NAME)=$(SETUP_ADDR) -o $@ $^\
		-l$(LINIT) -l$(LMACH) -l$(LARCH) -l$(LUTIL) -lgcc -lc -lgcc

install:	$(MACH)_setup
		$(STRIP) -o $<.tmp $<
		$(INSTALL) $<.tmp $(IMG)/$<
		$(CLEAN) $<.tmp

test:

clean:
		$(CLEAN) *.o *_setup
