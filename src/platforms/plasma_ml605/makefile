##################################################
# Project Setup
##################################################
TOP_MODULE = plasma_ml605
BUILD_DIR = build

##################################################
# Include other makefiles 
##################################################

# common definitions
include ../../../tools/xilinx/Makefile.common

# coregen sources definitions
include ../../../src/mach/common/coregen/hw.srcs

# other IP sources definitions
include ../../../src/mach/plasma/hw.srcs

##################################################
# Sources
##################################################

# Top level sources
TOP_SRCS = \
plasma_ml605.v \
plasma_ml605.ucf

# IP sources
SOURCES = $(abspath $(TOP_SRCS)) $(COREGEN_SRCS) \
$(PLASMA_SRCS)

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
