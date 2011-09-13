
module rtsnoc_echo (
    clk_i, rst_i,
    
    p0_din_o, p0_wr_o, p0_rd_o, p0_dout_i, p0_wait_i, p0_nd_i,
    p1_din_o, p1_wr_o, p1_rd_o, p1_dout_i, p1_wait_i, p1_nd_i
    );
   
   
    parameter P0_ADDR = 0;
    parameter P0_ADDR_X = 0;
    parameter P0_ADDR_Y = 0;
    parameter P1_ADDR = 1;
    parameter P1_ADDR_X = 0;
    parameter P1_ADDR_Y = 0;
    parameter SOC_SIZE_X = 1; //Log2
    parameter SOC_SIZE_Y = 1; //Log2
    parameter NOC_DATA_WIDTH = 16; 
    
    localparam SOC_XY_SIZE = (2*SOC_SIZE_Y)+(2*SOC_SIZE_X);
    localparam NOC_HEADER_SIZE = SOC_XY_SIZE + 6;
    localparam NOC_BUS_SIZE = NOC_DATA_WIDTH + NOC_HEADER_SIZE;
    
             
    //Ports
    input clk_i;
    input rst_i;
    
    output [37:0] p0_din_o;
    output p0_wr_o;
    output p0_rd_o;
    input [37:0] p0_dout_i;
    input p0_wait_i;
    input p0_nd_i;
    
    output [37:0] p1_din_o;
    output p1_wr_o;
    output p1_rd_o;
    input [37:0] p1_dout_i;
    input p1_wait_i;
    input p1_nd_i;
    
    rtsnoc_echo_sm #(
    	.TX_ADDR(P1_ADDR),
    	.TX_ADDR_X(P1_ADDR_X),
    	.TX_ADDR_Y(P1_ADDR_Y),
    	.SOC_SIZE_X(SOC_SIZE_X),
    	.SOC_SIZE_Y(SOC_SIZE_Y),
    	.NOC_DATA_WIDTH(NOC_DATA_WIDTH)
    )
    rtsnoc_echo_sm_0(
    	.clk_i(clk_i),
    	.rst_i(rst_i),
    	.dout_i(p0_dout_i),
    	.nd_i(p0_nd_i),
    	.rd_o(p0_rd_o),
    	.din_o(p1_din_o),
    	.wr_o(p1_wr_o),
    	.wait_i(p1_wait_i)
    );
    
    rtsnoc_echo_sm #(
        .TX_ADDR(P0_ADDR),
        .TX_ADDR_X(P0_ADDR_X),
        .TX_ADDR_Y(P0_ADDR_Y),
        .SOC_SIZE_X(SOC_SIZE_X),
        .SOC_SIZE_Y(SOC_SIZE_Y),
        .NOC_DATA_WIDTH(NOC_DATA_WIDTH)
    )
    rtsnoc_echo_sm_1(
        .clk_i(clk_i),
        .rst_i(rst_i),
        .dout_i(p1_dout_i),
        .nd_i(p1_nd_i),
        .rd_o(p1_rd_o),
        .din_o(p0_din_o),
        .wr_o(p0_wr_o),
        .wait_i(p0_wait_i)
    );
   
endmodule // timer

