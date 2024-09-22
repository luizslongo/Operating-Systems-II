# EPOS Application Makefile

include ../../makedefs

all: install

$(APPLICATION):	$(APPLICATION).o $(LIB)/*
		$(ALD) $(ALDFLAGS) -o $@ $(APPLICATION).o thread.o simulator.o scheduler.o

$(APPLICATION).o: $(APPLICATION).cc $(SRC)
		$(ACC) $(ACCFLAGS) -o $@ $<
		$(ACC) $(ACCFLAGS) -c src/thread.cc
		$(ACC) $(ACCFLAGS) -c src/simulator.cc
		$(ACC) $(ACCFLAGS) -c src/scheduler.cc

install: $(APPLICATION)
		$(INSTALL) $(APPLICATION) $(IMG)

clean:
		$(CLEAN) *.o $(APPLICATION)
