# EPOS Application Makefile

include ../makedefs

all: install

$(APPLICATION):	$(APPLICATION).o $(LIB)/* $(OBJS)
		$(ALD) $(ALDFLAGS) -o $@ $(APPLICATION).o $(OBJS)

$(APPLICATION).o: $(APPLICATION).cc $(SRC)
		$(ACC) $(ACCFLAGS) -o $@ $< $(ENABLE_THREADS)

install: $(APPLICATION)
ifndef  GDB
		$(STRIP) -o $(APPLICATION).tmp $<
else
		$(INSTALL) $< $(APPLICATION).tmp
endif
		$(INSTALL) $(APPLICATION).tmp $(IMG)/$(APPLICATION)
		$(CLEAN) $(APPLICATION).tmp

clean:
		$(CLEAN) *.o $(APPLICATION) $(OBJS)
