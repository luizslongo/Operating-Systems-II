# EPOS Image Makefile

include	../makedefs

IMAGE := $(addsuffix .img,$(APPLICATION))
OUTPUT := $(addsuffix .out,$(APPLICATION))

all:		$(IMAGE)

ifeq ($(NODES),1)
run:		$(IMAGE)
		$(EMULATOR) $(IMAGE) | $(TEE) $(OUTPUT)
else
run:		$(IMAGE)
		$(TERM) "$(EMULATOR) $(IMAGE) -net nic,model=pcnet,macaddr=56:34:12:0:54:52 -net socket,listen=:1234 | $(TEE) r-$(OUTPUT) && read -p 'Press [Enter] key to close ...'" &
		sleep 2
		$(TERM) "$(EMULATOR) $(IMAGE) -net nic,model=pcnet,macaddr=56:34:12:0:54:53 -net socket,connect=:1234 | $(TEE) s-$(OUTPUT) && read -p 'Press [Enter] key to close ...'" &
endif

debug:		$(IMAGE)
		$(EMULATOR) $(IMAGE) | $(TEE) $(OUTPUT) &
		$(DEBUGGER) $(APP)/$(APPLICATION)

clean:
		$(CLEAN) *_boot *_setup *_init *_system $(APPLICATION)
