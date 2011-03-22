`timescale 1ns / 1ps
////////////////////


module plasma_ml605 (
        // Misc, debug
        output [7:0] gpio_leds, //GPIO leds
        input  [7:0] gpio_sws, //GPIO switches
        output [4:0] dir_leds, //C,E,N,S,W leds
        input  [4:0] dir_btns, //C,E,N,S,W buttons
       
        // UART
        output uart_tx_o,
        input uart_rx_i,

        // Clocks
        input clk_fpga_p,  // Diff - 200 MHz
        input clk_fpga_n
    );
    

    wire clk_100MHz, clk_50MHz;

    // Main clock manager
    // Input: diff 200 MHz
    // Out1: 100 MHz
    // Out2: 50 MHz
    clk_xlnx_100M_diff clks_main (
        // Clock in ports
        .CLK_IN1_P          (clk_fpga_p),
        .CLK_IN1_N          (clk_fpga_n),
        // Clock out ports
        .CLK_OUT1           (clk_100MHz),
        .CLK_OUT2           (clk_50MHz) 
    );
     
   
    // Plasma itself
    wire reset = dir_btns[4];
    wire [18:0] gpio_others;

    // parameters
    plasma #(
        .memory_type("XILINX_16X"),
        .log_file("UNUSED"),
        .ethernet(0),
        .use_cache(1)
    )
    // component instantiation
    plasma (
        .clk(clk_50MHz),
        .reset(reset),
        .uart_write(uart_tx_o),
        .uart_read(uart_rx_i),

        .address(),
        .byte_we(),
        .data_write(),
        .data_read(32'b0),
        .mem_pause_in(1'b0),
        .no_ddr_start(),
        .no_ddr_stop(),

        .gpio0_out({gpio_others, dir_leds, gpio_leds}),
        .gpioA_in({19'b0, dir_btns, gpio_sws})
     );

endmodule
