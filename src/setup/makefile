# EPOS-- Setup Makefile

include $(EPOS)/makedefs

all:		install

pc_setup:	$(MACH)_setup.o 
		$(LD) $(LDFLAGS) -L$(CCLIB) -N -Ttext $(SETUP_ADDR) \
		-e $(SETUP_ADDR) -o $@ $< -l$(LUTIL) -l$(LSYS) \
		-l$(LINIT) -lgcc -lc -lgcc

pc_install:	pc_setup
		$(STRIP) -o $<.img $<


rcx_setup:	$(MACH)_setup.o 
		$(LD) $(LDFLAGS) -L$(CCLIB) -N -Ttext $(SETUP_ADDR) \
		-e $(SETUP_ADDR) -o $@ $< -l$(LUTIL) -l$(LSYS) \
		-l$(LINIT) -lgcc -lc -lgcc

rcx_install:	rcx_setup
		$(OBJCPY) -O binary $< $<.img


khomp_setup:	$(MACH)_setup.o 
		$(LD) $(LDFLAGS) -L$(CCLIB) -N -Ttext $(SETUP_ADDR) \
		-e $(SETUP_ADDR) -o $@ $< -l$(LUTIL) -l$(LSYS) \
		-l$(LINIT) -lgcc -lc -lgcc

khomp_install:	khomp_setup
	 	$(OBJCPY) -O binary $< $<.img


at90s_setup: FORCE
		$(TOUCH) $(MACH)_setup

at90s_install:  $(MACH)_setup
		$(TOUCH) $(MACH)_setup.img


at86rf_setup: FORCE
		$(TOUCH) $(MACH)_setup

at86rf_install: $(MACH)_setup
		$(TOUCH) $(MACH)_setup.img


install:        $(MACH)_install
		$(INSTALL) $(MACH)_setup.img $(IMG)/$(MACH)_setup
		$(CLEAN) $(MACH)_setup.img

test:

print:
		$(PRINT) $(INCLUDE)/system/memory_map.h \
		$(MACH)_setup.cc

clean:
		$(CLEAN) *.o $(MACH)_setup

FORCE:
