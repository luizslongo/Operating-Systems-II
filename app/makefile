# EPOS-- Application Makefile

include $(EPOS)/makedefs

all:		install

$(MACH)_loader:	$(MACH)_loader.o 
		$(ALD) $(ALDFLAGS) -o $@ $<

$(MACH)_loader.o: $(MACH)_loader.cc $(SRC)/system/$(MACH)_system
		$(ACC) $(ACCFLAGS) $<

install: 	$(MACH)_loader
		$(STRIP) -o $(MACH)_loader.img $<
		$(INSTALL) $(MACH)_loader.img $(IMG)/$(MACH)_loader
		$(CLEAN) $(MACH)_loader.img

clean:
		$(CLEAN) *.o $(MACH)_loader
