##################################################
# Project Setup
##################################################
TOP_MODULE = virtual_platform

##################################################
# Include other makefiles 
##################################################

# common definitions
include ../systemc_common/Makefile.common

# other IP sources definitions
include ../../arch/common/hw.srcs
include ../../arch/mips32/varch/hw.srcs
include ../../mach/rtsnoc/vmach/hw.srcs
include ../../mach/common/simple_uart/hw.srcs
include ../../mach/common/gpio/hw.srcs
include ../../mach/common/simple_pic/hw.srcs
include ../../mach/common/simple_timer/hw.srcs
include ../../mach/common/rtsnoc_router/hw.srcs

##################################################
# Sources
##################################################

# Top level sources
TOP_SRCS = \
virtual_platform.cc

# IP sources
SOURCES = $(abspath $(TOP_SRCS)) \
$(ARCH_COMMON_SC_SRCS) \
$(ARCH_MIPS32_VARCH_SC_SRCS) \
$(MACH_COMMON_SIMPLE_UART_SC_SRCS) \
$(MACH_COMMON_SIMPLE_PIC_SC_SRCS) \
$(MACH_COMMON_SIMPLE_TIMER_SC_SRCS) \
$(MACH_COMMON_GPIO_SC_SRCS) \
$(MACH_RTSNOC_VMACH_SC_SRCS)

# IP includes
INCLUDES = -I$(abspath $(BASE_DIR)/hw)

#Generate bin
$(BIN_FILE): $(SOURCES)
	$(CC) $(CC_FLAGS) -o $(BIN_FILE) $(SOURCES) $(SYS_INCLUDES) $(INCLUDES) $(SYS_LIBS_DIR) $(SYS_LIBS)  

