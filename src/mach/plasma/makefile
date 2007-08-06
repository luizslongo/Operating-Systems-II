# EPOS-- PC Machine Makefile

include ../../../makedefs

OBJS := $(subst .cc,.o,$(shell find *.cc | grep -v test | grep -v init))
INITS := $(subst .cc,.o,$(shell find *.cc | grep init))
TESTS := $(subst .cc,,$(shell find *.cc | grep test))

all:		$(LIBMACH) $(LIBINIT)

$(LIBMACH):	$(LIBMACH)($(OBJS))	

$(LIBINIT):	$(LIBINIT)($(INITS))

pc_interrupt.o	: pc_interrupt.cc
		$(CXX) $(CXXFLAGS) -fomit-frame-pointer $<

test:		$(TESTS)

clean:
		$(CLEAN) *.o *_test
