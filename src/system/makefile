# EPOS System Makefile

include ../../makedefs

OBJS := $(subst .cc,.o,$(shell find *.cc))

all:		install

$(MACH)_system_library: $(OBJS) $(LIBSYS)
		$(INSTALL) system_scaffold.o $(MACH)_system.o
		$(INSTALL) application_scaffold.o $(MACH)_application.o

$(MACH)_install_library: $(MACH)_system_library
		$(INSTALL) $(MACH)_system.o $(LIB)
		$(INSTALL) $(MACH)_application.o $(LIB)


install: $(MACH)_install_$(MODE)

clean:
		$(CLEAN) *.o *_system

FORCE:
