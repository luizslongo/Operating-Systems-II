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
plasma_ml605.ucf \
plasma_ml605.v \
#plasma_ml605.vhd



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
# Currently optmizing for speed
# If design does not fit, enable Resource Sharing

#SYNTHESIZE_PROPERTIES = \
"Optimization Goal" Speed \
"Optimization Effort" High \
"Resource Sharing" FALSE \
"Register Balancing" Yes \
"Optimize Instantiated Primitives" TRUE \
"Cross Clock Analysis" TRUE

#SYNTHESIZE_PROPERTIES = \
#"Optimization Goal" Speed \
#"Optimization Effort" High \

SYNTHESIZE_PROPERTIES = ""

TRANSLATE_PROPERTIES = ""

MAP_PROPERTIES = ""

#MAP_PROPERTIES = \
#"Placer Effort Level" High \
#"Placer Extra Effort" Normal \
#"Global Optimization" Speed \
#"Generate Detailed Map Report" TRUE \
#"Enable Multi-Threading" 2


PLACE_ROUTE_PROPERTIES = ""

#PLACE_ROUTE_PROPERTIES = \
#"Place & Route Effort Level (Overall)" High \
#"Extra Effort (Highest PAR level only)" Normal \
#"Enable Multi-Threading" 2

STATIC_TIMING_PROPERTIES = ""

GEN_PROG_FILE_PROPERTIES = ""

SIM_MODEL_PROPERTIES = ""
