# EPOS-- Init Makefile

include $(EPOS)/makedefs

all:		install

$(MACH)_init_builtin: $(EPOS)/src/system/$(MACH)_system init_first.o init.o $(LIBINIT)
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start .init=$(INIT_ADDR) \
		--entry=_init -o $(MACH)_init \
		$(EPOS)/lib/$(ARCH)_crti.o \
		$(EPOS)/lib/$(ARCH)_crtbegin.o \
		init_first.o init.o \
		$(EPOS)/lib/$(ARCH)_crtend.o \
		$(EPOS)/lib/$(ARCH)_crtn.o \
		-l$(LINIT) \
		-R $(EPOS)/src/system/$(MACH)_system \
		-l$(LUTIL) -lgcc -lc -lgcc

$(MACH)_init_library: init_first.o init.o


$(MACH)_install_builtin: $(MACH)_init_builtin
		$(STRIP) -o $(MACH)_init.img $(MACH)_init
		$(INSTALL) $(MACH)_init.img $(IMG)/$(MACH)_init
		$(CLEAN) $(MACH)_init.img

$(MACH)_install_library: $(MACH)_init_library

install:        $(MACH)_install_$(MODE)

test:

clean:
		$(CLEAN) *.o *_init

FORCE:
