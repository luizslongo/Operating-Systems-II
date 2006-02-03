# EPOS-- Image Makefile

include	$(EPOS)/makedefs

all:		epos.img

epos.img:	FORCE
		$(MKBI) $@ $(APPLICATION)

install:	all

clean		:
		$(CLEAN) *_boot *_setup *_system $(APPLICATION) *.img *.o

FORCE:
