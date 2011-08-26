# EPOS Application Makefile

include ../makedefs

ifeq ($(APPLICATION), dmec_app)
SRC := $(shell find ./dmec/src/ -mindepth 1 -name "*.cc" -type f)
OBJS := $(subst .cc,.do,$(SRC))
else
OBJS :=
endif

all:		install

.SUFFIXES: .do .cc

.cc.do :	$(SRC)
		$(CXX) $(CXXFLAGS) -o $@ $<

$(APPLICATION):	$(APPLICATION).o $(LIB)/* $(OBJS)
		$(ALD) $(ALDFLAGS) -o $@ $(APPLICATION).o $(OBJS)

$(APPLICATION).o: $(APPLICATION).cc $(SRC)
		$(ACC) $(ACCFLAGS) -o $@ $<

install: 	$(APPLICATION)
ifneq ($(GDB_DEBUG), 1)
		$(STRIP) -o $(APPLICATION).tmp $<
else
		cp $< $(APPLICATION).tmp
endif
		$(INSTALL) $(APPLICATION).tmp $(IMG)/$(APPLICATION)
		$(CLEAN) $(APPLICATION).tmp

clean:
		$(CLEAN) *.o $(APPLICATION) $(OBJS)
