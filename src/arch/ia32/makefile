# EPOS-- IA32 Architecture Makefile

include ../../../makedefs

OBJS := $(subst .cc,.o,$(shell find *.cc | grep -v init | grep -v test))
CRTS := $(subst .S,.o,$(shell find *.S | grep crt))
CRTSI := $(subst .S,.s,$(shell find *.S | grep crt))
INITS := $(subst .cc,.o,$(shell find *.cc | grep init))
TESTS := $(subst .cc,,$(shell find *.cc | grep test))

all:		crts $(LIBSYS) $(LIBINIT)

crts:		$(CRTS)
		$(INSTALL) $^ $(LIB)

.INTERMEDIATE:	$(CRTSI)

$(LIBSYS):	$(LIBSYS)($(OBJS))	

$(LIBINIT):	$(LIBINIT)($(INITS))

cpu.o		: cpu.cc
		$(CXX) $(CXXFLAGS) -fomit-frame-pointer $<

test:		$(TESTS)

clean:
		$(CLEAN) *.o *_test
