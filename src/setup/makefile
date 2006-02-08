# EPOS-- Setup Makefile

include $(EPOS)/makedefs

all:		install

$(MACH)_setup:	$(MACH)_setup.o
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start $(MACH_CODE_NAME)=$(SETUP_ADDR) \
		-o $@ $^ -l$(LSYS) -l$(LINIT) -l$(LUTIL) -lgcc -lc -lgcc

install:	$(MACH)_setup
		$(STRIP) -o $<.tmp $<
		$(INSTALL) $<.tmp $(IMG)/$<
		$(CLEAN) $<.tmp

test:

clean:
		$(CLEAN) *.o *_setup
