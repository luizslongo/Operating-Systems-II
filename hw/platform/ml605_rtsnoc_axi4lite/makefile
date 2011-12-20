##################################################
# Project Setup
##################################################
TOP_MODULE = ml605_rtsnoc_axi4lite
BUILD_DIR = build

##################################################
# Include other makefiles 
##################################################

# common definitions
include ../../../tools/xilinx/Makefile.common

# coregen sources definitions
include ../../mach/common/coregen/hw.srcs

# other IP sources definitions
include ../../arch/mips32/plasma/hw.srcs
include ../../mach/rtsnoc/axi4lite/hw.srcs
include ../../mach/common/amba/hw.srcs
include ../../mach/common/amba_wishbone/hw.srcs
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
ml605_rtsnoc_axi4lite.vhd \
ml605_rtsnoc_axi4lite.ucf


# Coregen cores
coregen_generate: \
clk_xlnx_100M_diff \
ram_amba_128k \
ram_amba_1024k \
chipscope_icon \
chipscope_ila
#axi4_ddr3_contrl

coregen_clean:
	cd $(COREGEN_DIR); \
	rm -rf \
	clk_xlnx_100M_diff \
	ram_amba_128k ; \
	rm -f coregen.cgc; \
	rm -rf tmp _xmsgs; \
	rm -f *.xise *.gise *.ncf *.tcl *.txt *.ejp *.log *.ngc *~ *.v *.veo *.vhd *.vho *.mif
#axi4_ddr3_contrl \



# IP sources
SOURCES = $(abspath $(TOP_SRCS)) \
$(ARCH_MIPS32_PLASMA_SRCS) \
$(MACH_COMMON_AMBA_SRCS) \
$(MACH_COMMON_AXI_WB_SRCS) \
$(MACH_COMMON_SIMPLE_UART_SRCS) \
$(MACH_COMMON_SIMPLE_PIC_SRCS) \
$(MACH_COMMON_SIMPLE_TIMER_SRCS) \
$(MACH_COMMON_COREGEN_SRCS) \
$(MACH_COMMON_RTSNOC_ROUTER_SRCS) \
$(MACH_COMMON_GPIO_SRCS) \
$(MACH_RTSNOC_AXI4LITE_SRCS) \




##################################################
# Project Properties
##################################################
PROJECT_PROPERTIES = \
family Virtex6 \
device xc6vlx240t \
package ff1156 \
speed -1 \
top_level_module_type "HDL" \
synthesis_tool "XST (VHDL/Verilog)" \
simulator "Modelsim-SE Mixed" \
"Preferred Language" "Verilog" \
"Enable Message Filtering" FALSE \
"Display Incremental Messages" FALSE 


##################################################
# Process Properties
##################################################
SYNTHESIZE_PROPERTIES = ""

TRANSLATE_PROPERTIES = ""

MAP_PROPERTIES = \
"Generate Detailed Map Report" TRUE \
"Enable Multi-Threading" 2

PLACE_ROUTE_PROPERTIES = \
"Enable Multi-Threading" 2

STATIC_TIMING_PROPERTIES = ""

GEN_PROG_FILE_PROPERTIES = ""

SIM_MODEL_PROPERTIES = ""
