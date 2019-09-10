# EPOS Setup Makefile

include ../../makedefs

TARGET := $(if $(shell find $(MMOD)* 2> /dev/null), $(if $(SETUP_ADDR), install, install.o))

all:	$(TARGET)

$(MMOD)_setup:	$(MMOD)_setup.o
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic --section-start .init=$(SETUP_ADDR) -o $@ $^ -l$(LINIT) -l$(LMACH) -l$(LARCH) -l$(LUTIL) -lgcc

install:	$(MMOD)_setup
		$(INSTALL) $< $(IMG)

install.o:	$(MMOD)_setup.o
		$(INSTALL) $< $(LIB)

clean:
		$(CLEAN) *.o *.s *_setup
