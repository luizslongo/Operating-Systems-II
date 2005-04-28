# EPOS-- Image Makefile
#
# Image layout (see command epos-mkbi):
# - boot  : skip 32 bytes header;
# - setup : skip ELF header
#           seek 512 bytes (boot)
# - kernel: seek 512 + 8K bytes (boot + setup)
# - loader: seek 512 + 8K + 32K + 16K (boot + setup + kernel + init)

include	$(EPOS)/makedefs

all:		$(MACH)_boot $(MACH)_setup $(MACH)_system $(APPLICATION)
		$(MKBI) $(MKBI_IMG) $(APPLICATION)

install:	all

clean		:
		$(CLEAN) *_boot *_setup *_system $(APPLICATION) *.img *.o

