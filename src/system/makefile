# EPOS-- System Makefile

include $(EPOS)/makedefs

all:		install

$(MACH)_system_builtin: scaffold.o $(LIBSYS)
		$(LD) $(LDFLAGS) -L$(CCLIB) --nmagic \
		--section-start $(MACH_CODE_NAME)=$(SYS_CODE_ADDR) \
		--section-start $(MACH_DATA_NAME)=$(SYS_DATA_ADDR) \
		--entry=__do_global_ctors_aux -o $(MACH)_system \
		$(EPOS)/lib/$(ARCH)_crti.o \
		$(EPOS)/lib/$(ARCH)_crtbegin.o \
		scaffold.o \
		$(EPOS)/lib/$(ARCH)_crtend.o \
		$(EPOS)/lib/$(ARCH)_crtn.o \
		--whole-archive -l$(LSYS) --no-whole-archive \
		-l$(LUTIL) -lgcc -lc -lgcc

$(MACH)_system_library: scaffold.o $(LIBSYS)
		$(INSTALL) scaffold.o $(MACH)_system.o


$(MACH)_install_builtin: $(MACH)_system_builtin
		$(STRIP) -o $(MACH)_system.tmp $(MACH)_system
		$(INSTALL) $(MACH)_system.tmp $(IMG)/$(MACH)_system
		$(CLEAN) $(MACH)_system.tmp

$(MACH)_install_library: $(MACH)_system_library
		$(INSTALL) $(MACH)_system.o $(LIB)


install:        $(MACH)_install_$(MODE)

test:

clean:
		$(CLEAN) *.o *_system

FORCE:
