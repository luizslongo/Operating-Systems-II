# EPOS-- Init Makefile

include $(EPOS)/makedefs

all:		install

$(MACH)_init_builtin: $(SRC)/system/$(MACH)_system init_first.o init_system.o $(LIBINIT)
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start .init=$(INIT_ADDR) \
		--entry=_init -o $(MACH)_init \
		$(EPOS)/lib/$(ARCH)_crti.o \
		$(EPOS)/lib/$(ARCH)_crtbegin.o \
		init_first.o init_system.o \
		$(EPOS)/lib/$(ARCH)_crtend.o \
		$(EPOS)/lib/$(ARCH)_crtn.o \
		-l$(LINIT) \
		-R $(SRC)/system/$(MACH)_system \
		-l$(LUTIL) -lgcc -lc -lgcc

$(MACH)_init_library: init_first.o init_system.o
		$(INSTALL) init_first.o $(MACH)_init_first.o
		$(INSTALL) init_system.o $(MACH)_init_system.o


$(MACH)_install_builtin: $(MACH)_init_builtin
		$(STRIP) -o $(MACH)_init.tmp $(MACH)_init
		$(INSTALL) $(MACH)_init.tmp $(IMG)/$(MACH)_init
		$(CLEAN) $(MACH)_init.tmp

$(MACH)_install_library: $(MACH)_init_library
		$(INSTALL) $(MACH)_init_first.o $(LIB)
		$(INSTALL) $(MACH)_init_system.o $(LIB)


install:        $(MACH)_install_$(MODE)

test:

clean:
		$(CLEAN) *.o *_init

FORCE:
