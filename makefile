# EPOS-- Main Makefile

include makedefs

SUBDIRS := cfg tools src app

all:		$(SUBDIRS) img

linux_imports:	FORCE
		@$(TCPP) $(TCPPFLAGS) -dD -P /usr/include/elf.h | \
			grep -v __ | $(SED) \
			-e s/'typedef int int32_t'/'typedef long int32_t'/g \
			-e s/'typedef unsigned int uint32_t'/'typedef unsigned long uint32_t'/g \
			-e s/'typedef int int_least32_t'/'typedef long int_least32_t'/g \
			-e s/'typedef unsigned int uint_least32_t'/'typedef unsigned long uint_least32_t'/g \
			-e s/'typedef int int_fast16_t'/'typedef long int_fast16_t'/g \
			-e s/'typedef int int_fast32_t'/'typedef long int_fast32_t'/g \
			-e s/'typedef unsigned int uintptr_t'/'typedef unsigned long uintptr_t'/g >! $(INCLUDE)/utility/elf-linux.h
		@$(TCPP) $(TCPPFLAGS) -dD -P /usr/include/linux/pci_ids.h | \
			grep -v __ >! $(INCLUDE)/system/pci_ids-linux.h

$(SUBDIRS):	FORCE
		(cd $@ && $(MAKE))

img:		FORCE
		(cd $@ && $(MAKE))

test:		$(SUBDIRS)
		(cd src && $(MAKETEST))

clean:		FORCE
		(cd src && $(MAKECLEAN))
		find $(LIB) -maxdepth 1 -type f -exec $(CLEAN) {} \;

veryclean:
		make MAKE:="$(MAKECLEAN)" $(SUBDIRS) img
		find $(LIB) -maxdepth 1 -type f -exec $(CLEAN) {} \;
		find $(BIN) -maxdepth 1 -type f -exec $(CLEAN) {} \;
		find $(APP) -maxdepth 1 -type f -perm +111 -exec $(CLEAN) {} \;
		find $(IMG) -maxdepth 1 -type f -perm +111 -exec $(CLEAN) {} \;
		find $(TOP) -name "*~" -exec $(CLEAN) {} \; 

dist:		veryclean
		find $(TOP) -name CVS -type d -print | xargs $(CLEANDIR)
		find $(TOP) -name "*.h" -print | xargs sed -i "1r $(TOP)/LICENSE" 
		find $(TOP) -name "*.cc" -print | xargs sed -i "1r $(TOP)/LICENSE" 

FORCE:
