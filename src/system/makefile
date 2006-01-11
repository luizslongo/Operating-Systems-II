# EPOS-- System Makefile

include $(EPOS)/makedefs

all:		install

$(MACH)_system_builtin: system.o $(LIBSYS)
		$(LD) $(LDFLAGS) -L$(CCLIB) --nmagic \
		--section-start $(MACH_CODE_NAME)=$(SYS_CODE_ADDR) \
		--section-start $(MACH_DATA_NAME)=$(SYS_DATA_ADDR) \
		--entry=__do_global_ctors_aux -o $(MACH)_system \
		$(EPOS)/lib/$(ARCH)_crti.o \
		$(EPOS)/lib/$(ARCH)_crtbegin.o \
		system.o \
		$(EPOS)/lib/$(ARCH)_crtend.o \
		$(EPOS)/lib/$(ARCH)_crtn.o \
		--whole-archive -l$(LSYS) --no-whole-archive \
		-l$(LUTIL) -lgcc -lc -lgcc

$(MACH)_system_library: system.o $(LIBSYS)
		$(TOUCH) $(MACH)_system

$(MACH)_install_builtin: $(MACH)_system_builtin
		$(STRIP) -o $(MACH)_system.img $(MACH)_system
		$(INSTALL) $(MACH)_system.img $(IMG)/$(MACH)_system
		$(CLEAN) $(MACH)_system.img

$(MACH)_install_library: $(MACH)_system_library
		$(TOUCH) $(MACH)_system.img
		$(INSTALL) $(MACH)_system.img $(IMG)/$(MACH)_system
		$(CLEAN) $(MACH)_system.img

install:        $(MACH)_install_$(MODE)

test:

clean:
		$(CLEAN) *.o *_system

FORCE:
