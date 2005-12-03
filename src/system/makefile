# EPOS-- System Makefile

include $(EPOS)/makedefs

all:		install

$(MACH)_system_kernel: init_thread.o initializer.o system.o $(LIBSYS)
		$(LD) $(LDFLAGS) -L$(CCLIB) \
		-n -Ttext $(SYS_CODE_ADDR) -Tdata $(SYS_DATA_ADDR) \
		-e $(SYS_CODE_ADDR) -o $(MACH)_system $< -l$(LSYS) \
		-l$(LUTIL) -lc -lgcc

$(MACH)_system_builtin: system.o $(LIBSYS)
		$(LD) $(LDFLAGS) -L$(CCLIB) --nmagic  \
		--section-start .init=$(SYS_CODE_ADDR) \
		--section-start .data=$(SYS_DATA_ADDR) \
		--entry=_init -o $(MACH)_system \
		system.o \
		--whole-archive -l$(LSYS) --no-whole-archive \
		-l$(LUTIL) -lgcc -lc -lgcc

$(MACH)_init_builtin: init_first.o init.o $(LIBINIT)
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start .init=$(INIT_ADDR) \
		--entry=_init -o $(MACH)_init \
		$(EPOS)/lib/$(ARCH)_crti.o \
		$(EPOS)/lib/$(ARCH)_crtbegin.o \
		init_first.o	init.o \
		$(EPOS)/lib/$(ARCH)_crtend.o \
		$(EPOS)/lib/$(ARCH)_crtn.o \
		--whole-archive -l$(LINIT) --no-whole-archive \
		-R $(MACH)_system \
		-l$(LUTIL) -lgcc -lc -lgcc

$(MACH)_system_library: init_first.o init.o system.o $(LIBSYS) $(LIBINIT)
		$(TOUCH) $(MACH)_system

$(MACH)_install_kernel: $(MACH)_init_kernel $(MACH)_system_kernel
		$(STRIP) -o $(MACH)_init.img $(MACH)_init
		$(INSTALL) $(MACH)_init.img $(IMG)/$(MACH)_init
		$(CLEAN) $(MACH)_init.img
		$(STRIP) -o $(MACH)_system.img $(MACH)_system
		$(INSTALL) $(MACH)_system.img $(IMG)/$(MACH)_system
		$(CLEAN) $(MACH)_system.img

$(MACH)_install_builtin: $(MACH)_system_builtin $(MACH)_init_builtin
		$(STRIP) -o $(MACH)_system.img $(MACH)_system
		$(INSTALL) $(MACH)_system.img $(IMG)/$(MACH)_system
		$(CLEAN) $(MACH)_system.img
		$(STRIP) -o $(MACH)_init.img $(MACH)_init
		$(INSTALL) $(MACH)_init.img $(IMG)/$(MACH)_init
		$(CLEAN) $(MACH)_init.img

$(MACH)_install_library: $(MACH)_system_library
		$(TOUCH) $(MACH)_system.img
		$(INSTALL) $(MACH)_system.img $(IMG)/$(MACH)_system
		$(CLEAN) $(MACH)_system.img

install:        $(MACH)_install_$(MODE)

test:

clean:
		$(CLEAN) *.o *_init *_system

FORCE:
