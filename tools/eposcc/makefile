# EPOSCC Makefile

include	../../makedefs

all:		install

install:	eposcc
		$(INSTALL) -m 775 eposcc $(BIN)
		sed -e 's/^EPOS=.*$$/EPOS=$(subst /,\/,$(EPOS))/' -i $(BIN)/eposcc
		@echo "ARCH=$(ARCH)" > $(ETC)/eposcc.conf
		@echo "MACH=$(MACH)" >> $(ETC)/eposcc.conf
		@echo "TOOLS_PREFIX=$(COMP_PREFIX)" >> $(ETC)/eposcc.conf
		@echo "APP_CODE=$(APP_CODE_ADDR)" >> $(ETC)/eposcc.conf
		@echo "APP_DATA=$(APP_DATA_ADDR)" >> $(ETC)/eposcc.conf
		@echo "MACH_CC_FLAGS='$(MACH_CC_FLAGS)'" >> $(ETC)/eposcc.conf
		@echo "MACH_LD_FLAGS='$(MACH_LD_FLAGS)'" >> $(ETC)/eposcc.conf
		@echo "MACH_CODE_NAME='$(MACH_CODE_NAME)'" >> $(ETC)/eposcc.conf
		@echo "MACH_DATA_NAME='$(MACH_DATA_NAME)'" >> $(ETC)/eposcc.conf

clean:
