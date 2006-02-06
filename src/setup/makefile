# EPOS-- Setup Makefile

include $(EPOS)/makedefs

all:		$(MACH)_install

pc_setup:	pc_setup_start.o pc_setup.o
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start $(MACH_CODE_NAME)=$(SETUP_ADDR) \
		--entry=$(SETUP_ADDR) \
		-o $@ $^ -l$(LSYS) -l$(LINIT) -l$(LUTIL) -lgcc -lc -lgcc

pc_install:	pc_setup
		$(STRIP) -o $<.tmp $<
		$(INSTALL) $(MACH)_setup.tmp $(IMG)/$(MACH)_setup
		$(CLEAN) $(MACH)_setup.tmp


atmega16_install:
atmega128_install:


test:

clean:
		$(CLEAN) *.o *_setup
