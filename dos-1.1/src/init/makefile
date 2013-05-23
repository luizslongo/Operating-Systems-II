# EPOS Init Makefile

include ../../makedefs

OBJS := $(subst .cc,.o,$(shell find *.cc))

all:		install

$(MACH)_init_library: $(OBJS)
		$(INSTALL) init_first.o $(MACH)_init_first.o
		$(INSTALL) init_system.o $(MACH)_init_system.o
		$(INSTALL) init_application.o $(MACH)_init_application.o


$(MACH)_install_library: $(MACH)_init_library
		$(INSTALL) $(MACH)_init_first.o $(LIB)
		$(INSTALL) $(MACH)_init_system.o $(LIB)
		$(INSTALL) $(MACH)_init_application.o $(LIB)


install:        $(MACH)_install_$(MODE)

clean:
		$(CLEAN) *.o *_init

FORCE:
