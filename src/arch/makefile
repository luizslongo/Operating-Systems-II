# EPOS-- Architecture Mediators Makefile

include		$(EPOS)/makedefs

SUBDIRS := common $(ARCH)

all:		$(SUBDIRS)

$(SUBDIRS):	FORCE
		(cd $@ && $(MAKE))

test:
		make MAKE:="$(MAKETEST)" $(SUBDIRS)

clean:
		make MAKE:="$(MAKECLEAN)" $(SUBDIRS)

FORCE:
