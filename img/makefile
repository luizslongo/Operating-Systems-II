# EPOS-- Image Makefile

include	../makedefs

all:		$(APPLICATION).img

$(APPLICATION).img: $(APPLICATION)
		$(MKBI) $@ $(APPLICATION)

run:		$(APPLICATION).img
		$(QEMU) -fda $(APPLICATION).img | tee $(APPLICATION).out

clean		:
		$(CLEAN) *_boot *_setup *_system $(APPLICATION) *.o

FORCE:
