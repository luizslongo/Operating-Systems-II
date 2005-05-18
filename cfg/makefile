# EPOS-- Configuration Makefile

include $(EPOS)/makedefs

all:		eposcc.conf eposmkbi.conf


eposcc.conf:	
		echo "ARCH=$(ARCH)" >! $@
		echo "MACH=$(MACH)" >> $@
		echo "TOOLS_PREFIX=$(COMP_PREFIX)" >> $@
		echo "APP_CODE=$(APP_CODE_ADDR)" >> $@
		echo "APP_DATA=$(APP_DATA_ADDR)" >> $@

eposmkbi.conf:	
		echo "MODE=$(MODE)" >! $@
		echo "MACH=$(MACH)" >> $@
		echo "ARCH=$(ARCH)" >> $@
		echo "CLOCK=$(ARCH_CLOCK)" >> $@
		echo "WORD_SIZE=$(ARCH_WORD_SIZE)" >> $@
		echo "ENDIANESS=$(ARCH_ENDIANESS)" >> $@
		echo "MEM_BASE=$(MEM_BASE)" >> $@
		echo "MEM_SIZE=$(MEM_SIZE)" >> $@

clean:
		$(CLEAN) *.conf
