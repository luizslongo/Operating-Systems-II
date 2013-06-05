# EPOS Abstractions Makefile

include ../../makedefs

# Roda.
CXXFLAGS := $(MACH_CC_FLAGS) -nostdinc -Wall -I$(INCLUDE) -g

OBJS := $(subst .cc,.o,$(shell find *.cc | grep -v _test | grep -v _init))
INITS := $(subst .cc,.o,$(shell find *.cc | grep _init))

all:		$(LIBSYS) $(LIBINIT)

$(LIBSYS):	$(LIBSYS)($(OBJS))	

$(LIBINIT):	$(LIBINIT)($(INITS))

clean:
		$(CLEAN) *.o *_test
