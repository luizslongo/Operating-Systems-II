# EPOS Main Makefile

include makedefs

SUBDIRS	:= etc tools src app img

all: FORCE
ifndef APPLICATION
		$(foreach app,$(APPLICATIONS),$(MAKE) APPLICATION=$(app) $(PRECLEAN) prebuild_$(app) all1 posbuild_$(app);)
		$(MAKE) clean1
else
		$(MAKE) all1
endif

all1: $(SUBDIRS)

$(SUBDIRS): FORCE
		(cd $@ && $(MAKE))

run: FORCE
ifndef APPLICATION
		$(foreach app,$(APPLICATIONS),$(MAKE) APPLICATION=$(app) $(PRECLEAN) prerun_$(app) run1;)
else
		$(MAKE) run1
endif

run1: img/$(APPLICATION)$(MACH_IMGSUFF)
		(cd img && $(MAKE) run1)
		
img/$(APPLICATION)$(MACH_IMGSUFF):
		$(MAKE) $(PRECLEAN) all1
		
runall: FORCE
		(cd img && $(MAKE) runall)

debug: FORCE
ifndef APPLICATION
		$(foreach app,$(APPLICATIONS),$(MAKE) DEBUG=1 APPLICATION=$(app) $(PRECLEAN) all1 debug1;)
else
		$(MAKE) DEBUG=1 all1 debug1
endif

debug1: FORCE
		(cd img && $(MAKE) DEBUG=1 debug)

flash: FORCE
ifndef APPLICATION
		$(foreach app,$(APPLICATIONS),$(MAKE) APPLICATION=$(app) $(PRECLEAN) flash1;)
else
		$(MAKE) flash1
endif

flash1: all1
		(cd img && $(MAKE) flash)

TESTS		:= $(shell find $(TST) -maxdepth 1 -type d -and -not -name tests -printf "%f\n")
TESTS_COMPILED 	:= $(subst .img,,$(shell find $(IMG) -name \*.img -printf "%f\n"))
TESTS_COMPILED 	:= $(TESTS_COMPILED) $(subst .bin,,$(shell find $(IMG) -name \*.bin -printf "%f\n"))
TESTS_FINISHED 	:= $(subst .out,,$(shell find $(IMG) -name \*.out -printf "%f\n"))
UNFINISHED_TESTS:= $(filter-out $(TESTS_FINISHED),$(TESTS))
UNCOMPILED_TESTS:= $(filter-out $(TESTS_COMPILED),$(TESTS))
test: FORCE
		$(foreach tst,$(TESTS),$(LINK) $(TST)/$(tst) $(APP);)
		$(foreach tst,$(UNFINISHED_TESTS),$(MAKETEST) APPLICATION=$(tst) prebuild_$(tst) clean1 all1 posbuild_$(tst) prerun_$(tst) run1 posbuild_$(tst);)
		
buildtest: FORCE
		$(foreach tst,$(TESTS),$(LINK) $(TST)/$(tst) $(APP);)
		$(foreach tst,$(UNCOMPILED_TESTS),$(MAKETEST) APPLICATION=$(tst) prebuild_$(tst) clean1 all1 posbuild_$(tst);)

runtest: FORCE
		$(foreach tst,$(TESTS),$(LINK) $(TST)/$(tst) $(APP);)
		$(foreach tst,$(UNFINISHED_TESTS),$(MAKETEST) APPLICATION=$(tst) etc prerun_$(tst) run1 posbuild_$(tst);)

cleantest: FORCE
		$(foreach tst,$(TESTS),$(LINK) $(TST)/$(tst) $(APP);)
		$(foreach tst,$(TESTS),cd $(TST)/${tst} && $(MAKE) APPLICATION=$(tst) clean;)
		find $(APP) -maxdepth 1 -type l -exec $(CLEAN) {} \;

.PHONY: prebuild_$(APPLICATION) posbuild_$(APPLICATION) prerun_$(APPLICATION)
prebuild_$(APPLICATION):
		@echo "Building $(APPLICATION) ..."
posbuild_$(APPLICATION):
		@echo "done!"
prerun_$(APPLICATION):
#		@echo "Cooling down for 10s ..."
#		sleep 10
		@echo "Running $(APPLICATION) ..."

clean: FORCE
ifndef APPLICATION
		$(MAKE) APPLICATION=$(word 1,$(APPLICATIONS)) clean1
else
		$(MAKE) clean1
endif

clean1: FORCE
		(cd etc && $(MAKECLEAN))
		(cd app && $(MAKECLEAN))
		(cd src && $(MAKECLEAN))
		(cd img && $(MAKECLEAN))
		find $(LIB) -maxdepth 1 -type f -not -name .gitignore -exec $(CLEAN) {} \;

cleanapps: FORCE
		$(foreach app,$(APPLICATIONS),cd $(APP)/${app} && $(MAKE) APPLICATION=$(app) clean;)

veryclean: clean cleanapps cleantest
		(cd tools && $(MAKECLEAN))
		find $(BIN) -maxdepth 1 -type f -not -name .gitignore -exec $(CLEAN) {} \;
		find $(IMG) -name "*.img" -exec $(CLEAN) {} \;
		find $(IMG) -name "*.bin" -exec $(CLEAN) {} \;
		find $(IMG) -name "*.hex" -exec $(CLEAN) {} \;
		find $(IMG) -name "*.out" -exec $(CLEAN) {} \;
		find $(IMG) -name "*.pcap" -exec $(CLEAN) {} \;
		find $(IMG) -name "*.net" -exec $(CLEAN) {} \;
		find $(IMG) -maxdepth 1 -type f -perm 755 -exec $(CLEAN) {} \;

dist: veryclean
		find $(TOP) -name ".*project" -exec $(CLEAN) {} \;
		find $(TOP) -name CVS -type d -print | xargs $(CLEANDIR)
		find $(TOP) -name .svn -type d -print | xargs $(CLEANDIR)
		find $(TOP) -name "*.h" -print | xargs sed -i "1r $(TOP)/LICENSE"
		find $(TOP) -name "*.cc" -print | xargs sed -i "1r $(TOP)/LICENSE"
		sed -e 's/^\/\//#/' LICENSE > LICENSE.mk
		find $(TOP) -name "makedefs" -print | xargs sed -i "1r $(TOP)/LICENSE.mk"
		find $(TOP) -name "makefile" -print | xargs sed -i "1r $(TOP)/LICENSE.mk"
		$(CLEAN) LICENSE.mk
		sed -e 's/^\/\//#/' LICENSE > LICENSE.as
		find $(TOP) -name "*.S" -print | xargs sed -i "1r $(TOP)/LICENSE.as"
		$(CLEAN) LICENSE.as

FORCE:
