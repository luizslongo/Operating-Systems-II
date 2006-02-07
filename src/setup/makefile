# EPOS-- Setup Makefile

include $(EPOS)/makedefs

all:		install

$(MACH)_setup:	$(MACH)_setup_start.o $(MACH)_setup.o
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start $(MACH_CODE_NAME)=$(SETUP_ADDR) \
		--entry=$(SETUP_ADDR) \
		-o $@ $(MACH)_setup_start.o $(MACH)_setup.o -l$(LUTIL) -l$(LSYS) -l$(LINIT) -lgcc -lc -lgcc

$(MACH)_install:$(MACH)_setup
		$(STRIP) -o $<.img $<


install:        $(MACH)_install
		$(INSTALL) $(MACH)_setup.img $(IMG)/$(MACH)_setup
		$(CLEAN) $(MACH)_setup.img

test:


clean:
		$(CLEAN) *.o *_setup
