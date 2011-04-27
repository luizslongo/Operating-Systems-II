# EPOS Application Makefile

include ../makedefs

all:		install


$(APPLICATION):	$(APPLICATION).o $(LIB)/*
		$(ALD) $(ALDFLAGS) -o $@ $<

$(APPLICATION).o: $(APPLICATION).cc 
		$(ACC) $(ACCFLAGS) -o $@ $<

install: 	$(APPLICATION)
ifneq ($(GDB_DEBUG), 1)
		$(STRIP) -o $(APPLICATION).tmp $<
else
		cp $< $(APPLICATION).tmp
endif
		$(INSTALL) $(APPLICATION).tmp $(IMG)/$(APPLICATION)
		$(CLEAN) $(APPLICATION).tmp

clean:
		$(CLEAN) *.o $(APPLICATION)
