#clear stuff
dataset clear
profile clear
vdel -all

#create work library
vlib work

#compile vhdl/verilog files for mixed simulation
vcom ../../mach/plasma/mlite_pack.vhd
vcom ../../mach/plasma/mlite_cpu.vhd
vcom ../../mach/plasma/alu.vhd
vcom ../../mach/plasma/bus_mux.vhd
vcom ../../mach/plasma/control.vhd
vcom ../../mach/plasma/mem_ctrl.vhd
vcom ../../mach/plasma/mult.vhd
vcom ../../mach/plasma/pipeline.vhd
vcom ../../mach/plasma/pc_next.vhd
vcom ../../mach/plasma/reg_bank.vhd
vcom ../../mach/plasma/shifter.vhd
vcom ../../mach/plasma/cache.vhd
vcom plasma_axi4lite_testbench.vhd

#load/dump/run vhdl design
vsim work.plasma_axi4lite_testbench
vcd file trace.vcd

vcd add plasma_axi4lite_testbench/clk_50MHz

vcd add plasma_axi4lite_testbench/sig_arvalid
vcd add plasma_axi4lite_testbench/sig_arready
vcd add plasma_axi4lite_testbench/sig_araddr
vcd add plasma_axi4lite_testbench/sig_arprot

vcd add plasma_axi4lite_testbench/sig_rvalid
vcd add plasma_axi4lite_testbench/sig_rready
vcd add plasma_axi4lite_testbench/sig_rdata
vcd add plasma_axi4lite_testbench/sig_rresp

run -all
