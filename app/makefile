# EPOS-- Application Makefile

include $(EPOS)/makedefs

all:		install


$(APPLICATION):	$(APPLICATION).o
		$(ALD) $(ALDFLAGS) -o $@ $<

$(APPLICATION).o: $(APPLICATION).cc $(SRC)/system/$(MACH)_system
		$(ACC) $(ACCFLAGS) -o $@ $<

install: 	$(APPLICATION)
		$(STRIP) -o $(APPLICATION).img $<
		$(INSTALL) $(APPLICATION).img $(IMG)/$(APPLICATION)
		$(CLEAN) $(APPLICATION).img

clean:
		$(CLEAN) *.o $(APPLICATION)
