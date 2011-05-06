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
vcom ../../mach/plasma/plasma_axi4lite_master.vhd

vcom ../../mach/common/coregen/ram_amba_128k.vhd

vlog ../../mach/common/axi_uart/axi_uart_xilinx.v

vlog ../../mach/common/amba/amba_mux.v
vlog ../../mach/common/amba/address_decoder.v
vlog ../../mach/common/amba/priority_encoder.v
vlog ../../mach/common/amba/axi4lite_decoder.v

vcom plasma_axi4lite_dec_testbench.vhd


#load/dump/run vhdl design
vsim -L unisim -L axi_uartlite_v1_01_a  work.plasma_axi4lite_testbench
vcd file trace_dec.vcd

vcd add -r -file trace_dec.vcd /*

run -all
