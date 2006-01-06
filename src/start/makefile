# EPOS-- Startup Files Makefile

include $(EPOS)/makedefs

all: 		$(ARCH)_crt1.o $(ARCH)_crti.o $(ARCH)_crtbegin.o \
		$(ARCH)_crtend.o $(ARCH)_crtn.o
		$(INSTALL) $^ $(LIB)

.INTERMEDIATE:	$(ARCH)_crt1.s $(ARCH)_crti.s $(ARCH)_crtbegin.s \
		$(ARCH)_crtend.s $(ARCH)_crtn.s

test:

clean:
		$(CLEAN) *.o
