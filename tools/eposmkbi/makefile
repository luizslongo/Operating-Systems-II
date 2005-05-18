# eposmkbi Makefile

include	$(EPOS)/makedefs

all		: install

eposmkbi	: eposmkbi.cc
		$(TCXX) $(TCXXFLAGS) $<
		$(TLD) $(TLDFLAGS) -o $@ eposmkbi.o

install		: eposmkbi
		$(INSTALL) $^ $(BIN)

clean		:
		$(CLEAN) *.o eposmkbi
