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


avrmcu_setup:	$(MACH)_setup.o
		$(LD) $(LDFLAGS) -L$(CCLIB) -N -Ttext $(SETUP_ADDR) \
		-e $(SETUP_ADDR) -o $@ $< -l$(LUTIL) -l$(LSYS) \
		-l$(LINIT) -lgcc -lc -lgcc

avrmcu_install: $(MACH)_setup
		$(TOUCH) $(MACH)_setup.img

mica2_setup:	$(MACH)_setup.o
		$(LD) $(LDFLAGS) -L$(CCLIB) -N -Ttext $(SETUP_ADDR) \
		-e $(SETUP_ADDR) -o $@ $< -l$(LUTIL) -l$(LSYS) \
		-l$(LINIT) -lgcc -lc -lgcc

mica2_install: 	$(MACH)_setup
		$(TOUCH) $(MACH)_setup.img


install:        $(MACH)_install
		$(INSTALL) $(MACH)_setup.img $(IMG)/$(MACH)_setup
		$(CLEAN) $(MACH)_setup.img

test:

print:
		$(PRINT) $(INCLUDE)/system/memory_map.h \
		$(MACH)_setup.cc

clean:
		$(CLEAN) *.o *_setup

FORCE:
