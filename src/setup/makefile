# EPOS-- Setup Makefile

include $(EPOS)/makedefs

all:		install

$(MACH)_setup:	$(MACH)_setup.o
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start $(MACH_CODE_NAME)=$(SETUP_ADDR) \
		-o $@ $< -l$(LUTIL) -l$(LSYS) -l$(LINIT) -lgcc -lc -lgcc

$(MACH)_install:$(MACH)_setup
		$(STRIP) -o $<.tmp $<


install:        $(MACH)_install
		$(INSTALL) $(MACH)_setup.tmp $(IMG)/$(MACH)_setup
		$(CLEAN) $(MACH)_setup.tmp

test:


clean:
		$(CLEAN) *.o *_setup
