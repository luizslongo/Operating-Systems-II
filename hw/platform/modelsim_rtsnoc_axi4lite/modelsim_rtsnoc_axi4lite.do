#clear stuff
dataset clear
profile clear
vdel -all

#create work library
vlib work

#compile vhdl/verilog files for mixed simulation
vcom ../../arch/mips32/plasma/mlite_pack.vhd
vcom ../../arch/mips32/plasma/mlite_cpu.vhd
vcom ../../arch/mips32/plasma/alu.vhd
vcom ../../arch/mips32/plasma/bus_mux.vhd
vcom ../../arch/mips32/plasma/control.vhd
vcom ../../arch/mips32/plasma/mem_ctrl.vhd
vcom ../../arch/mips32/plasma/mult.vhd
vcom ../../arch/mips32/plasma/pipeline.vhd
vcom ../../arch/mips32/plasma/pc_next.vhd
vcom ../../arch/mips32/plasma/reg_bank.vhd
vcom ../../arch/mips32/plasma/shifter.vhd
vcom ../../arch/mips32/plasma/plasma_axi4lite_master.vhd

vcom ../../mach/common/coregen/ram_amba_128k.vhd
vcom ../../mach/common/coregen/ram_amba_1024k.vhd

vlog ../../mach/common/amba/amba_mux.v
vlog ../../mach/common/amba/address_decoder.v
vlog ../../mach/common/amba/priority_encoder.v
vlog ../../mach/common/amba/axi4_reset_control.v
vcom ../../mach/common/amba/axi4lite_dummy_master.vhd
vlog ../../mach/common/amba/axi4lite_decoder.v

vlog ../../mach/common/amba_wishbone/axi4lite_to_wishbone.v
vcom ../../mach/common/amba_wishbone/wishbone_to_axi4lite.vhd

vlog ../../mach/common/simple_uart/shortfifo.v
vlog ../../mach/common/simple_uart/medfifo.v
vlog ../../mach/common/simple_uart/simple_uart_rx.v
vlog ../../mach/common/simple_uart/simple_uart_tx.v
#vlog ../../mach/common/simple_uart/simple_uart.v
sccom -g ../../mach/common/simple_uart/simple_uart.cc
vlog ../../mach/common/simple_uart/simple_uart_axi4lite.v

vlog ../../mach/common/gpio/very_simple_gpio.v
vlog ../../mach/common/gpio/gpio_axi4lite.v

vlog ../../mach/common/simple_pic/priority_enc.v
vlog ../../mach/common/simple_pic/pic.v
vlog ../../mach/common/simple_pic/pic_axi4lite.v

vlog ../../mach/common/simple_timer/timer2.v
vlog ../../mach/common/simple_timer/timer_axi4lite.v

vcom ../../mach/common/rtsnoc_router/router/ARBITER_MACHINE.vhd
vcom ../../mach/common/rtsnoc_router/router/COMPARE.vhd
vcom ../../mach/common/rtsnoc_router/router/crossbar.vhd
vcom ../../mach/common/rtsnoc_router/router/FLOW_CONTROL.vhd
vcom ../../mach/common/rtsnoc_router/router/INPUT_INTERFACE.vhd
vcom ../../mach/common/rtsnoc_router/router/OUTPUT_INTERFACE.vhd
vcom ../../mach/common/rtsnoc_router/router/PIPELINE.vhd
vcom ../../mach/common/rtsnoc_router/router/PRIORITY.vhd
vcom ../../mach/common/rtsnoc_router/router/QUEUE.vhd
vcom ../../mach/common/rtsnoc_router/router/ROUTER.vhd
vlog ../../mach/common/rtsnoc_router/rtsnoc_wishbone_proxy.v
vlog ../../mach/common/rtsnoc_router/rtsnoc_axi4lite_proxy.v
vlog ../../mach/common/rtsnoc_router/rtsnoc_axi4lite_reset.v
vlog ../../mach/common/rtsnoc_router/rtsnoc_echo_sm.v
vlog ../../mach/common/rtsnoc_router/rtsnoc_echo.v
vlog ../../mach/common/rtsnoc_router/rtsnoc_to_wishbone_master.v
vlog ../../mach/common/rtsnoc_router/wishbone_slave_to_rtsnoc.v
vlog ../../mach/common/rtsnoc_router/axi4lite_slave_to_rtsnoc.v
vlog ../../mach/common/rtsnoc_router/rtsnoc_to_axi4lite_master.v

vcom ../../mach/rtsnoc/axi4lite/axi4lite_proc_io_node.vhd
vcom ../../mach/rtsnoc/axi4lite/axi4lite_old.vhd

vlog ../../mach/common/simple_uart/uart_rx.v

#top models
vlog uart_rx_wrapper.v

#export vhdl modules for systemc
scgenmod axi4lite_old > axi4lite_old.h
scgenmod axi4_reset_control > axi4_reset_control.h
scgenmod uart_rx_wrapper > uart_rx_wrapper.h

#toplevel
sccom -g modelsim_rtsnoc_axi4lite.cc
vcom modelsim_rtsnoc_axi4lite.vhd

#link systemc with modelsim
sccom -L /usr/lib -l util -link 

#load/dump/run vhdl design
set StdArithNoWarnings 1
set NumericStdNoWarnings 1

#vsim -L unisim -L unisims_ver -L xilinxcorelib  work.virtual_platform
vsim -L unisim -L unisims_ver -L xilinxcorelib  work.virtual_platform_hdl

#vcd file trace_dec.vcd
#vcd add -r -file trace_dec.vcd /*
#vcd add -r -file trace_dec.vcd /plasma_axi4lite_testbench/plasma/timer/*
#vcd add -r -file trace_dec.vcd /plasma_axi4lite_testbench/plasma/pic/*
#vcd add -file trace_dec.vcd /plasma_axi4lite_testbench/plasma/plasma_amba/*

run -all
