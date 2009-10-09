# EPOSCC Makefile

include	../../makedefs

all:		install

eposcc.conf:	
		@echo "ARCH=$(ARCH)" >! $@
		@echo "MACH=$(MACH)" >> $@
		@echo "TOOLS_PREFIX=$(COMP_PREFIX)" >> $@
		@echo "APP_CODE=$(APP_CODE_ADDR)" >> $@
		@echo "APP_DATA=$(APP_DATA_ADDR)" >> $@
		@echo "MACH_CC_FLAGS='$(MACH_CC_FLAGS)'" >> $@
		@echo "MACH_LD_FLAGS='$(MACH_LD_FLAGS)'" >> $@
		@echo "MACH_CODE_NAME='$(MACH_CODE_NAME)'" >> $@
		@echo "MACH_DATA_NAME='$(MACH_DATA_NAME)'" >> $@

install:	eposcc eposcc.conf
		chmod a+x eposcc
		$(INSTALL) eposcc $(BIN)
		$(INSTALL) eposcc.conf $(ETC)

clean		:
		$(CLEAN) eposcc.conf
