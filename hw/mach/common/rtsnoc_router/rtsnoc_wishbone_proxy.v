
//Memory map
//  0x00 - NoC header (RW)
//  0x04 - NoC header (RW)
//  0x08 - NoC header (RW)
//  0x0C - NoC header (RW)
//  0x10 - NoC header (RW)
//  0x14 - NoC header (RW)
//  0x18 - NoC data (RW)
//  0x1C - NoC Status 
//      wb_dat_o[0] = noc_wr_o (RW) (write here to transmit)
//      wb_dat_o[1] = noc_rd_o (RW) (write here to ack an rx)
//      wb_dat_o[2] = noc_wait_i (R-only)
//      wb_dat_o[3] = noc_nd_i (R-only)
//  0x20 - NoC local address (R-only)
//      wb_dat_o[2:0] = 0x0 (NN)
//                      0x1 (NE)
//                      0x2 (EE)
//                      0x3 (SE)
//                      0x4 (SS)
//                      0x5 (SW)
//                      0x6 (WW)
//                      0x7 (NW)
//  0x24 - NoC X address (R-only)
//  0x28 - NoC Y address (R-only
//  0x2C - NoC X size (Log2) (R-only)
//  0x30 - NoC Y size (Log2) (R-only)
//  0x34 - NoC data width (R-only)

module rtsnoc_wishbone_proxy (
    clk_i, rst_i,
     
    wb_cyc_i, wb_stb_i, wb_adr_i, wb_sel_i, wb_we_i, wb_dat_i, wb_dat_o, wb_ack_o,
    
    noc_int_o,
    
    noc_din_o, noc_wr_o, noc_rd_o, noc_dout_i, noc_wait_i, noc_nd_i
    );
   
    //Parameters
    localparam WB_ADDR_WIDTH = 6;
    localparam WB_DATA_WIDTH = 32;
    
    parameter NOC_LOCAL_ADR = 0;
    parameter NOC_X = 0;
    parameter NOC_Y = 0;
    parameter SOC_SIZE_X = 1; //Log2
    parameter SOC_SIZE_Y = 1; //Log2
    parameter NOC_DATA_WIDTH = 16; 
    
    localparam SOC_XY_SIZE = (2*SOC_SIZE_Y)+(2*SOC_SIZE_X);
    localparam NOC_HEADER_SIZE = SOC_XY_SIZE + 6;
    localparam NOC_BUS_SIZE = NOC_DATA_WIDTH + NOC_HEADER_SIZE;
    
              
                    
    //Ports
    input clk_i;
    input rst_i;
    input wb_cyc_i;
    input wb_stb_i;
    input [WB_ADDR_WIDTH-1:0] wb_adr_i;
    input [3:0] wb_sel_i;
    input wb_we_i;
    input [WB_DATA_WIDTH-1:0] wb_dat_i;
    output reg [WB_DATA_WIDTH-1:0] wb_dat_o;
    output reg wb_ack_o;
    
    output reg noc_int_o;
  
    output [NOC_BUS_SIZE-1:0] noc_din_o;
    output reg noc_wr_o;
    output reg noc_rd_o;
    input [NOC_BUS_SIZE-1:0] noc_dout_i;
    input noc_wait_i;
    input noc_nd_i;
    

    wire [NOC_DATA_WIDTH-1:0] noc_rx_data;
    wire [2:0] noc_rx_local_dst;
    wire [SOC_SIZE_Y-1:0] noc_rx_Y_dst;
    wire [SOC_SIZE_X-1:0] noc_rx_X_dst;
    wire [2:0] noc_rx_local_orig;
    wire [SOC_SIZE_Y-1:0] noc_rx_Y_orig;
    wire [SOC_SIZE_X-1:0] noc_rx_X_orig;
    assign {noc_rx_X_orig,
            noc_rx_Y_orig,
            noc_rx_local_orig, 
            noc_rx_X_dst,
            noc_rx_Y_dst,
            noc_rx_local_dst,
            noc_rx_data} = noc_dout_i[NOC_BUS_SIZE-1:0];
    
    reg [NOC_DATA_WIDTH-1:0] noc_tx_data;
    reg [2:0] noc_tx_local_dst;
    reg [SOC_SIZE_Y-1:0] noc_tx_Y_dst;
    reg [SOC_SIZE_X-1:0] noc_tx_X_dst;
    reg [2:0] noc_tx_local_orig;
    reg [SOC_SIZE_Y-1:0] noc_tx_Y_orig;
    reg [SOC_SIZE_X-1:0] noc_tx_X_orig;
    assign noc_din_o[NOC_BUS_SIZE-1:0] = {noc_tx_X_orig,
                                          noc_tx_Y_orig,
                                          noc_tx_local_orig, 
                                          noc_tx_X_dst,
                                          noc_tx_Y_dst,
                                          noc_tx_local_dst,
                                          noc_tx_data};
    
    localparam WB_NOC_DATA_DIFF = WB_DATA_WIDTH - NOC_DATA_WIDTH;
    localparam WB_NOC_HEAD_DIFF = WB_DATA_WIDTH - NOC_HEADER_SIZE;
   
    always @(posedge clk_i)
        if(rst_i) begin
	       wb_dat_o <= 0;
	       wb_ack_o <= 0;
	       noc_wr_o <= 0;
	       noc_rd_o <= 0;
	       
	       noc_tx_data <= 0;
           noc_tx_local_dst <= 0;
           noc_tx_Y_dst <= 0;
           noc_tx_X_dst <= 0;
           noc_tx_local_orig <= 0;
           noc_tx_Y_orig <= 0;
           noc_tx_X_orig <= 0;
        end
        else begin
	       if(wb_stb_i & wb_cyc_i) begin
	           if(wb_we_i) begin
	               case (wb_adr_i[WB_ADDR_WIDTH-1:2])
                   0: begin
                       noc_tx_local_dst <= wb_dat_i[2:0];
                   end
                   1: begin
                       noc_tx_Y_dst <= wb_dat_i[SOC_SIZE_Y-1:0];
                   end
                   2: begin
                       noc_tx_X_dst <= wb_dat_i[SOC_SIZE_X-1:0];
                   end
                   3: begin
                       noc_tx_local_orig <= wb_dat_i[2:0];
                   end
                   4: begin
                       noc_tx_Y_orig <= wb_dat_i[SOC_SIZE_Y-1:0];
                   end
                   5: begin
                       noc_tx_X_orig <= wb_dat_i[SOC_SIZE_X-1:0];
                   end
                   6: begin
                       noc_tx_data <= wb_dat_i[NOC_DATA_WIDTH-1:0];
                   end
                   7: begin
                       noc_wr_o <= wb_dat_i[0];
                       noc_rd_o <= wb_dat_i[1];
                   end
                   default: begin
                   end
                   endcase
	           end
	           else begin
	               case (wb_adr_i[WB_ADDR_WIDTH-1:2])
                   0: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= 
                            {{(WB_ADDR_WIDTH-2){1'b0}},
                             noc_rx_local_dst};
                   end
                   1: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= 
                            {{(WB_ADDR_WIDTH-SOC_SIZE_Y){1'b0}},
                             noc_rx_Y_dst};
                   end
                   2: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= 
                            {{(WB_ADDR_WIDTH-SOC_SIZE_X){1'b0}},
                             noc_rx_X_dst};
                   end
                   3: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= 
                            {{(WB_ADDR_WIDTH-2){1'b0}},
                             noc_rx_local_orig};
                   end
                   4: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= 
                            {{(WB_ADDR_WIDTH-SOC_SIZE_Y){1'b0}},
                             noc_rx_Y_orig};
                   end
                   5: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= 
                            {{(WB_ADDR_WIDTH-SOC_SIZE_X){1'b0}},
                             noc_rx_X_orig};
                   end
                   6: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= {{WB_NOC_DATA_DIFF{1'b0}},noc_rx_data};
                   end
                   7: begin
                       wb_dat_o[0] <= noc_wr_o;
                       wb_dat_o[1] <= noc_rd_o;
                       wb_dat_o[2] <= noc_wait_i;
                       wb_dat_o[3] <= noc_nd_i;
                       wb_dat_o[WB_DATA_WIDTH-1:4] <= 0;
                   end
                   8: begin
                       wb_dat_o[2:0] <= NOC_LOCAL_ADR;
                       wb_dat_o[WB_DATA_WIDTH-1:3] <= 0;
                   end
                   9: begin
                       wb_dat_o[SOC_SIZE_X-1:0] <= NOC_X;
                       wb_dat_o[WB_DATA_WIDTH-1:SOC_SIZE_X] <= 0;
                   end
                   10: begin
                       wb_dat_o[SOC_SIZE_Y-1:0] <= NOC_Y;
                       wb_dat_o[WB_DATA_WIDTH-1:SOC_SIZE_Y] <= 0;
                   end
                   11: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= SOC_SIZE_X;
                   end
                   12: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= SOC_SIZE_Y;
                   end
                   13: begin
                       wb_dat_o[WB_DATA_WIDTH-1:0] <= NOC_DATA_WIDTH;
                   end
                   default: begin
                   end
                   endcase
	           end
	           wb_ack_o <= 1'b1;
	       end
	       else begin
	           noc_wr_o <= 1'b0;
	           noc_rd_o <= 1'b0;
	           wb_ack_o <= 1'b0;
	       end 
        end
        
    
    reg noc_nd;
    always @(posedge clk_i)
        if(rst_i) begin
           noc_int_o <= 1'b0;
           noc_nd <= 1'b0;
        end
        else begin
            noc_nd <= noc_nd_i;
            noc_int_o <= noc_nd_i & ~noc_nd;        
        end

   
endmodule // timer

