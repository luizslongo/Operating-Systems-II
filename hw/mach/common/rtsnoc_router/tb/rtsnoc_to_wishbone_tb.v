`timescale 1ns / 1ps

module rtsnoc_to_wishbone_tb;

    parameter WB_ADDR_WIDTH = 6;
    parameter WB_NOC_DATA_WIDTH = 32;
    parameter NOC_LOCAL_ADR = 3'd0;
    parameter NOC_X = 1'd0;
    parameter NOC_Y = 1'd0;
    parameter NOC_LOCAL_ADR_TGT = 3'd1;
    parameter NOC_X_TGT = 1'd0;
    parameter NOC_Y_TGT = 1'd0;
    parameter SOC_SIZE_X = 1; //Log2
    parameter SOC_SIZE_Y = 1; //Log2
    
    localparam SOC_XY_SIZE = (2*SOC_SIZE_Y)+(2*SOC_SIZE_X);
    localparam NOC_HEADER_SIZE = SOC_XY_SIZE + 6;
    localparam NOC_BUS_SIZE = WB_NOC_DATA_WIDTH + NOC_HEADER_SIZE;
    
    reg slave_cyc = 0;
    reg slave_stb = 0;
    reg [WB_ADDR_WIDTH-1:0] slave_adr = 0;
    reg [3:0] slave_sel = 0;
    reg slave_we = 0;
    reg [WB_NOC_DATA_WIDTH-1:0] slave_dat_i = 0;
    wire [WB_NOC_DATA_WIDTH-1:0] slave_dat_o;
    wire slave_ack;
    wire slave_int;
    
    wire master_cyc;
    wire master_stb;
    wire [WB_ADDR_WIDTH-1:0] master_adr;
    wire [3:0] master_sel;
    wire master_we;
    wire [WB_NOC_DATA_WIDTH-1:0] master_dat_o;
    reg [WB_NOC_DATA_WIDTH-1:0] master_dat_i = 0;
    reg master_ack = 0;
    reg master_int = 0;
    
    wire [NOC_BUS_SIZE-1:0] slave_din;
    wire slave_wr;
    wire slave_rd;
    reg [NOC_BUS_SIZE-1:0] slave_dout = 0;
    reg slave_wait_reg = 0;
    wire slave_wait = slave_wr | slave_wait_reg;
    reg slave_nd = 0;
    
    wire [NOC_BUS_SIZE-1:0] master_din;
    wire master_wr;
    wire master_rd;
    reg [NOC_BUS_SIZE-1:0] master_dout = 0;
    reg master_wait_reg = 0;
    wire master_wait = master_wr | master_wait_reg;
    reg master_nd = 0;
    
    reg clk = 0;
    always #500 clk = ~clk;
    
    reg rst = 1;
    
    wishbone_slave_to_rtsnoc #(
    	.WB_ADDR_WIDTH(WB_ADDR_WIDTH),
    	.WB_NOC_DATA_WIDTH(WB_NOC_DATA_WIDTH),
    	.NOC_LOCAL_ADR(NOC_LOCAL_ADR),
    	.NOC_X(NOC_X),
    	.NOC_Y(NOC_Y),
    	.NOC_LOCAL_ADR_TGT(NOC_LOCAL_ADR_TGT),
    	.NOC_X_TGT(NOC_X_TGT),
    	.NOC_Y_TGT(NOC_Y_TGT),
    	.SOC_SIZE_X(SOC_SIZE_X),
    	.SOC_SIZE_Y(SOC_SIZE_Y)
    )
    wishbone_slave_to_rtsnoc(
    	.clk_i(clk),
    	.rst_i(rst),
    	.wb_cyc_i(slave_cyc),
    	.wb_stb_i(slave_stb),
    	.wb_adr_i(slave_adr),
    	.wb_sel_i(slave_sel),
    	.wb_we_i(slave_we),
    	.wb_dat_i(slave_dat_i),
    	.wb_dat_o(slave_dat_o),
    	.wb_ack_o(slave_ack),
    	.noc_int_o(slave_int),
    	.noc_din_o(slave_din),
    	.noc_wr_o(slave_wr),
    	.noc_rd_o(slave_rd),
    	.noc_dout_i(slave_dout),
    	.noc_wait_i(slave_wait),
    	.noc_nd_i(slave_nd)
    );
    
    rtsnoc_to_wishbone_master #(
    	.WB_ADDR_WIDTH(WB_ADDR_WIDTH),
        .WB_NOC_DATA_WIDTH(WB_NOC_DATA_WIDTH),
        .NOC_LOCAL_ADR(NOC_LOCAL_ADR_TGT),
        .NOC_X(NOC_X_TGT),
        .NOC_Y(NOC_Y_TGT),
        .NOC_LOCAL_ADR_TGT(NOC_LOCAL_ADR),
        .NOC_X_TGT(NOC_X),
        .NOC_Y_TGT(NOC_Y),
        .SOC_SIZE_X(SOC_SIZE_X),
        .SOC_SIZE_Y(SOC_SIZE_Y)
    )
    rtsnoc_to_wishbone_master(
    	.clk_i(clk),
    	.rst_i(rst),
    	.wb_cyc_o(master_cyc),
    	.wb_stb_o(master_stb),
    	.wb_adr_o(master_adr),
    	.wb_sel_o(master_sel),
    	.wb_we_o(master_we),
    	.wb_dat_o(master_dat_o),
    	.wb_dat_i(master_dat_i),
    	.wb_ack_i(master_ack),
    	.int_i(master_int),
    	.noc_din_o(master_din),
    	.noc_wr_o(master_wr),
    	.noc_rd_o(master_rd),
    	.noc_dout_i(master_dout),
    	.noc_wait_i(master_wait),
    	.noc_nd_i(master_nd)
    );
    
    //NoC stuff
    wire [WB_NOC_DATA_WIDTH-1:0] master_dout_data;
    wire [2:0] master_dout_local_dst;
    wire [SOC_SIZE_Y-1:0] master_dout_Y_dst;
    wire [SOC_SIZE_X-1:0] master_dout_X_dst;
    wire [2:0] master_dout_local_orig;
    wire [SOC_SIZE_Y-1:0] master_dout_Y_orig;
    wire [SOC_SIZE_X-1:0] master_dout_X_orig;
    assign {master_dout_X_orig,
            master_dout_Y_orig,
            master_dout_local_orig, 
            master_dout_X_dst,
            master_dout_Y_dst,
            master_dout_local_dst,
            master_dout_data} = master_dout;
    wire [WB_NOC_DATA_WIDTH-1:0] master_din_data;
    wire [2:0] master_din_local_dst;
    wire [SOC_SIZE_Y-1:0] master_din_Y_dst;
    wire [SOC_SIZE_X-1:0] master_din_X_dst;
    wire [2:0] master_din_local_orig;
    wire [SOC_SIZE_Y-1:0] master_din_Y_orig;
    wire [SOC_SIZE_X-1:0] master_din_X_orig;
    assign {master_din_X_orig,
            master_din_Y_orig,
            master_din_local_orig, 
            master_din_X_dst,
            master_din_Y_dst,
            master_din_local_dst,
            master_din_data} = master_din;
    wire [WB_NOC_DATA_WIDTH-1:0] slave_dout_data;
    wire [2:0] slave_dout_local_dst;
    wire [SOC_SIZE_Y-1:0] slave_dout_Y_dst;
    wire [SOC_SIZE_X-1:0] slave_dout_X_dst;
    wire [2:0] slave_dout_local_orig;
    wire [SOC_SIZE_Y-1:0] slave_dout_Y_orig;
    wire [SOC_SIZE_X-1:0] slave_dout_X_orig;
    assign {slave_dout_X_orig,
            slave_dout_Y_orig,
            slave_dout_local_orig, 
            slave_dout_X_dst,
            slave_dout_Y_dst,
            slave_dout_local_dst,
            slave_dout_data} = slave_dout;
    wire [WB_NOC_DATA_WIDTH-1:0] slave_din_data;
    wire [2:0] slave_din_local_dst;
    wire [SOC_SIZE_Y-1:0] slave_din_Y_dst;
    wire [SOC_SIZE_X-1:0] slave_din_X_dst;
    wire [2:0] slave_din_local_orig;
    wire [SOC_SIZE_Y-1:0] slave_din_Y_orig;
    wire [SOC_SIZE_X-1:0] slave_din_X_orig;
    assign {slave_din_X_orig,
            slave_din_Y_orig,
            slave_din_local_orig, 
            slave_din_X_dst,
            slave_din_Y_dst,
            slave_din_local_dst,
            slave_din_data} = slave_din;                        
    

    
    reg master_tx_free = 1;
    always begin
        @(posedge clk);
        if(slave_rd)
            master_tx_free = 1;
    end
    always begin
        @(posedge clk);
        if(master_wr) begin
            $display ("rtsnoc_to_wishbone_master:\t Sending PKT,start");
            master_wait_reg = 1'b1;
            @(posedge clk);
            @(posedge clk);
            if((master_din_X_dst == NOC_X) &&
               (master_din_Y_dst == NOC_Y) &&
               (master_din_local_dst == NOC_LOCAL_ADR) &&
               (master_din_X_orig == NOC_X_TGT) &&
               (master_din_Y_orig == NOC_Y_TGT) &&
               (master_din_local_orig == NOC_LOCAL_ADR_TGT)) begin
                
                $display ("rtsnoc_to_wishbone_master:\t Sending PKT waiting");
                while(~master_tx_free) @(posedge clk);
                slave_dout = master_din;
                slave_nd = 1'b1;
                master_wait_reg = 1'b0;
                master_tx_free = 0;
                @(posedge clk);
                slave_nd = 1'b0;
                $display ("rtsnoc_to_wishbone_master:\t Sending PKT,end");
            end
            else begin
                master_wait_reg = 1'b0;
                $display ("rtsnoc_to_wishbone_master:\t Sending PKT wrong addr");
            end
        end
        
    
    end
    
    reg slave_tx_free = 1;
    always begin
        @(posedge clk);
        if(master_rd)
            slave_tx_free = 1;
    end
    always begin
        @(posedge clk);
        if(slave_wr) begin
            $display ("wishbone_slave_to_rtsnoc:\t Sending PKT,start");
            slave_wait_reg <= 1'b1;
            @(posedge clk);
            @(posedge clk);
            if((slave_din_X_dst == NOC_X_TGT) &&
               (slave_din_Y_dst == NOC_Y_TGT) &&
               (slave_din_local_dst == NOC_LOCAL_ADR_TGT) &&
               (slave_din_X_orig == NOC_X) &&
               (slave_din_Y_orig == NOC_Y) &&
               (slave_din_local_orig == NOC_LOCAL_ADR)) begin
                
                $display ("wishbone_slave_to_rtsnoc:\t Sending PKT waiting");
                while(~slave_tx_free) @(posedge clk);
                master_dout = slave_din;
                master_nd = 1'b1;
                slave_wait_reg = 1'b0;
                slave_tx_free = 0;
                @(posedge clk);
                master_nd = 1'b0;
                $display ("wishbone_slave_to_rtsnoc:\t Sending PKT,end");
            end
            else begin
                slave_wait_reg = 1'b0;
                $display ("wishbone_slave_to_rtsnoc:\t Sending PKT wrong addr");
            end
        end
    end
    
    
    
    
    //Testbench
    
    
    reg slave_start = 0;
    initial begin
        
        $dumpfile("rtsnoc_to_wishbone_tb.lxt");
        $dumpvars(0,rtsnoc_to_wishbone_tb);
    
        $display ("Waiting some cycles");
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        $display("Rst going down");
        rst = 0;
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        @(posedge clk);
        $display ("Starting slave");
        slave_start = 1;
    end
    
    reg [31:0] data = 32'hAABBCCDD;
    
    reg [WB_ADDR_WIDTH-1:0] counter = 0;
    integer max = 5;
    
    initial begin
        
        while(slave_start == 0) @(posedge clk);
        
        for(counter = 0; counter < max; counter = counter + 1) begin
            $display ("Starting slave write transaction: data %d, addr %d",data, counter);
            @(posedge clk)
            slave_adr = counter;
            slave_cyc = 1;
            slave_stb = 1;
            slave_we = 1;
            slave_dat_i = data;
            data = {data[15:0],data[31:16]};
            @(posedge clk);
            slave_cyc = 0;
            slave_stb = 0;
            slave_we = 0;
            while(~slave_ack) @(posedge clk); 
        end
        
        for(counter = 0; counter < max; counter = counter + 1) begin
            $display ("Starting slave read transaction: addr %d", counter);
            @(posedge clk)
            slave_adr = counter;
            slave_cyc = 1;
            slave_stb = 1;
            @(posedge clk);
            slave_cyc = 0;
            slave_stb = 0;
            while(~slave_ack) @(posedge clk);
            $display ("Read from slave %d",slave_dat_o); 
        end
        
        
        for(counter = 0; counter < max; counter = counter + 1) begin
            $display ("Starting slave read transaction: addr %d", counter);
            @(posedge clk)
            slave_adr = counter;
            slave_cyc = 1;
            slave_stb = 1;
            @(posedge clk);
            slave_cyc = 0;
            slave_stb = 0;
            while(~slave_ack) @(posedge clk);
            $display ("Read from slave %d",slave_dat_o);
            
            @(posedge clk);
            $display ("Starting slave write transaction: data %d, addr %d",data, counter);
            @(posedge clk)
            slave_adr = counter;
            slave_cyc = 1;
            slave_stb = 1;
            slave_we = 1;
            slave_dat_i = data;
            data = {data[15:0],data[31:16]};
            @(posedge clk);
            slave_cyc = 0;
            slave_stb = 0;
            slave_we = 0;
            while(~slave_ack) @(posedge clk);  
        end
        
        $display($time, "Finish called.");
        $finish;
    
    end
    
    reg [31:0] data2 = 32'hEEEEFFFF;
    
    always begin
         @(posedge clk);
         master_ack = 0;
         if(master_stb & master_cyc) begin
            if(master_we) 
                $display ("Master wants to write data %d to addr %d",master_dat_o, master_adr);
            else begin 
                $display ("Master wants to read %d from addr %d",data2, master_adr);
                master_dat_i = data2;
                data2 = {data2[15:0],data2[31:16]};
            end
            @(posedge clk);
            master_ack = 1;
         end
    end

   
endmodule

