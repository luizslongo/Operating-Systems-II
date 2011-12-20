library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.log2;
use ieee.math_real.ceil;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- Memory Map (X:Y:LOCAL):
--  Proc node   0:0:0x000
--  Echo P0     0:0:0x110
--  Echo P1     0:0:0x100

entity axi4lite is
   generic(
        CLK_FREQ : integer := 50_000_000);

   port(clk_i       : in std_logic;
        reset_i     : in std_logic;

        uart_tx_o   : out std_logic;
        uart_rx_i   : in std_logic;
        uart_baud_o : out std_logic;
        
        gpio_i      : in  std_logic_vector(31 downto 0);
        gpio_o      : out  std_logic_vector(31 downto 0);
        ext_int_i   : in std_logic_vector(7 downto 0));

end axi4lite;

architecture RTL of axi4lite is
    

    -- RTSNoC constants
        -- Dimension of the network and router address.
    constant NET_SIZE_X         : integer := 1;
    constant NET_SIZE_Y         : integer := 1;
    constant NET_SIZE_X_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_X))))" when NET_SIZE_X >= 2
    constant NET_SIZE_Y_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_Y))))" when NET_SIZE_Y >= 2
    constant ROUTER_X           : std_logic_vector(NET_SIZE_X_LOG2-1 downto 0) := "0";
    constant ROUTER_Y           : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := "0";
    constant NET_DATA_WIDTH     : integer := 32;
    constant ROUTER_N_PORTS     : integer := 8;
    constant NET_BUS_SIZE       : integer := NET_DATA_WIDTH+(2*NET_SIZE_X_LOG2)+(2*NET_SIZE_Y_LOG2)+6;
    --constant NET_BUS_SIZE_INV   : integer := 64 - NET_BUS_SIZE;
        -- Local addresses (clockwise: nn, ne, ee, se, ss, sw, ww, nw)
    type router_addr_type is array (0 to ROUTER_N_PORTS-1) of std_logic_vector(2 downto 0);
    constant ROUTER_ADDRS   : router_addr_type := 
            ("000", "001", "010", "011", "100", "101", "110", "111");
    constant ROUTER_NN  : integer := 0;
    constant ROUTER_NE  : integer := 1;
    constant ROUTER_EE  : integer := 2;
    constant ROUTER_SE  : integer := 3;
    constant ROUTER_SS  : integer := 4;
    constant ROUTER_SW  : integer := 5;
    constant ROUTER_WW  : integer := 6;
    constant ROUTER_NW  : integer := 7;
        -- NoC node addressess
    constant NODE_AXI_ADDR  : std_logic_vector(2 downto 0) := ROUTER_ADDRS(ROUTER_NN);
    constant NODE_ECHO_P0   : std_logic_vector(2 downto 0) := ROUTER_ADDRS(ROUTER_WW);
    constant NODE_ECHO_P1   : std_logic_vector(2 downto 0) := ROUTER_ADDRS(ROUTER_SS);
    
    -- 
    -- Declarations
    --

    COMPONENT ROUTER
        GENERIC (
            p_X         : integer := conv_integer(ROUTER_X);
            p_Y         : integer := conv_integer(ROUTER_Y);
            p_DATA      : integer := NET_DATA_WIDTH;                    
            p_SIZE_X    : integer := NET_SIZE_X_LOG2;
            p_SIZE_Y    : integer := NET_SIZE_Y_LOG2);  
        PORT(
            o_TESTE     : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_CLK       : IN std_logic;
            i_RST       : IN std_logic;
            i_DIN_NN    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_NN     : IN std_logic;
            i_RD_NN     : IN std_logic;
            i_DIN_NE    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_NE     : IN std_logic;
            i_RD_NE     : IN std_logic;
            i_DIN_EE    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_EE     : IN std_logic;
            i_RD_EE     : IN std_logic;
            i_DIN_SE    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_SE     : IN std_logic;
            i_RD_SE     : IN std_logic;
            i_DIN_SS    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_SS     : IN std_logic;
            i_RD_SS     : IN std_logic;
            i_DIN_SW    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_SW     : IN std_logic;
            i_RD_SW     : IN std_logic;
            i_DIN_WW    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_WW     : IN std_logic;
            i_RD_WW     : IN std_logic;
            i_DIN_NW    : IN std_logic_vector(NET_BUS_SIZE-1 downto 0);
            i_WR_NW     : IN std_logic;
            i_RD_NW     : IN std_logic;          
            o_DOUT_NN   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_NN   : OUT std_logic;
            o_ND_NN     : OUT std_logic;
            o_DOUT_NE   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_NE   : OUT std_logic;
            o_ND_NE     : OUT std_logic;
            o_DOUT_EE   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_EE   : OUT std_logic;
            o_ND_EE     : OUT std_logic;
            o_DOUT_SE   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_SE   : OUT std_logic;
            o_ND_SE     : OUT std_logic;
            o_DOUT_SS   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_SS   : OUT std_logic;
            o_ND_SS     : OUT std_logic;
            o_DOUT_SW   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_SW   : OUT std_logic;
            o_ND_SW     : OUT std_logic;
            o_DOUT_WW   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_WW   : OUT std_logic;
            o_ND_WW     : OUT std_logic;
            o_DOUT_NW   : OUT std_logic_vector(NET_BUS_SIZE-1 downto 0);
            o_WAIT_NW   : OUT std_logic;
            o_ND_NW     : OUT std_logic
        );
    END COMPONENT;
    
    component rtsnoc_axi4lite_reset is
        port (
            axi_rst_i   : in std_logic;
            noc_reset_o : out std_logic    
        );
    end component;
    
    component axi4lite_proc_node is
        generic(
            CLK_FREQ : integer := 50_000_000;
            -- RTSNoC generics
            NET_SIZE_X         : integer := 1;
            NET_SIZE_Y         : integer := 1;
            NET_SIZE_X_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_X))))" when NET_SIZE_X >= 2
            NET_SIZE_Y_LOG2    : integer := 1; -- it should be "integer(ceil(log2(real(NET_SIZE_Y))))" when NET_SIZE_Y >= 2
            NET_DATA_WIDTH     : integer := NET_DATA_WIDTH;
            NET_BUS_SIZE       : integer := NET_BUS_SIZE;
            ROUTER_X_ADDR      : integer := 0;
            ROUTER_Y_ADDR      : integer := 0;
            ROUTER_LOCAL_ADDR  : std_logic_vector(2 downto 0) := "0");
        port(clk_i       : in std_logic;
            reset_axi_i     : in std_logic;
            reset_noc_i     : in std_logic;
    
            uart_tx_o   : out std_logic;
            uart_rx_i   : in std_logic;
            uart_baud_o : out std_logic;
        
            gpio_i      : in  std_logic_vector(31 downto 0);
            gpio_o      : out  std_logic_vector(31 downto 0);
            ext_int_i   : in std_logic_vector(7 downto 0);
        
            noc_din_o   : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            noc_wr_o    : out std_logic;
            noc_rd_o    : out std_logic;
            noc_dout_i  : in std_logic_vector(NET_BUS_SIZE-1 downto 0);
            noc_wait_i  : in std_logic;
            noc_nd_i    : in std_logic;
        
            ext_ram_awaddr_o  : out std_logic_vector(31 DOWNTO 0);
            ext_ram_awvalid_o : out std_logic;
            ext_ram_awready_i : in std_logic;
            ext_ram_wdata_o   : out std_logic_vector(31 DOWNTO 0);
            ext_ram_wstrb_o   : out std_logic_vector(3 DOWNTO 0);
            ext_ram_wvalid_o  : out std_logic;
            ext_ram_wready_i  : in std_logic;
            ext_ram_bresp_i   : in std_logic_vector(1 DOWNTO 0);
            ext_ram_bvalid_i  : in std_logic;
            ext_ram_bready_o  : out std_logic;
            ext_ram_araddr_o  : out std_logic_vector(31 DOWNTO 0);
            ext_ram_arvalid_o : out std_logic;
            ext_ram_arready_i : in std_logic;
            ext_ram_rdata_i   : in std_logic_vector(31 DOWNTO 0);
            ext_ram_rresp_i   : in std_logic_vector(1 DOWNTO 0);
            ext_ram_rvalid_i  : in std_logic;
            ext_ram_rready_o  : out std_logic);
    end component;
      
    component ram_amba_1024k is
        port(
            s_aclk        : in std_logic;
            s_aresetn     : in std_logic;
            s_axi_awaddr  : in std_logic_vector(31 downto 0);
            s_axi_awvalid : in std_logic;
            s_axi_awready : out std_logic;
            s_axi_wdata   : in std_logic_vector(31 downto 0);
            s_axi_wstrb   : in std_logic_vector(3 downto 0);
            s_axi_wvalid  : in std_logic;
            s_axi_wready  : out std_logic;
            s_axi_bresp   : out std_logic_vector(1 downto 0);
            s_axi_bvalid  : out std_logic;
            s_axi_bready  : in std_logic;
            s_axi_araddr  : in std_logic_vector(31 downto 0);
            s_axi_arvalid : in std_logic;
            s_axi_arready : out std_logic;
            s_axi_rdata   : out std_logic_vector(31 downto 0);
            s_axi_rresp   : out std_logic_vector(1 downto 0);
            s_axi_rvalid  : out std_logic;
            s_axi_rready  : in std_logic);
    end component;

    component rtsnoc_echo is
        generic (
            SOC_SIZE_X      : integer := NET_SIZE_X_LOG2;
            SOC_SIZE_Y      : integer := NET_SIZE_Y_LOG2;
            NOC_DATA_WIDTH  : integer := NET_DATA_WIDTH;
            P0_ADDR   : std_logic_vector(2 downto 0) := NODE_ECHO_P0;
            P1_ADDR   : std_logic_vector(2 downto 0) := NODE_ECHO_P1);
        port(
            clk_i      : in std_logic;
            rst_i      : in std_logic;
            p0_din_o   : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            p0_dout_i  : in std_logic_vector(NET_BUS_SIZE-1 downto 0);
            p0_wr_o    : out std_logic;
            p0_rd_o    : out std_logic;
            p0_wait_i  : in std_logic;
            p0_nd_i    : in std_logic;
            p1_din_o   : out std_logic_vector(NET_BUS_SIZE-1 downto 0);
            p1_dout_i  : in std_logic_vector(NET_BUS_SIZE-1 downto 0);
            p1_wr_o    : out std_logic;
            p1_rd_o    : out std_logic;
            p1_wait_i  : in std_logic;
            p1_nd_i    : in std_logic);
    end component;
    
    -- Debugging cores
    --component chipscope_icon
    --    PORT (
    --        CONTROL0 : INOUT STD_LOGIC_VECTOR(35 DOWNTO 0));
    --end component;
    --
    --component chipscope_ila
    --    PORT (
    --        CONTROL : INOUT STD_LOGIC_VECTOR(35 DOWNTO 0);
    --        CLK : IN STD_LOGIC;
    --        DATA : IN STD_LOGIC_VECTOR(255 DOWNTO 0);
    --        TRIG0 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    --        TRIG1 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    --        TRIG2 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    --        TRIG3 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    --        TRIG4 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    --        TRIG5 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    --        TRIG6 : IN STD_LOGIC_VECTOR(0 DOWNTO 0);
    --        TRIG7 : IN STD_LOGIC_VECTOR(0 DOWNTO 0));
    --end component;


    --  
    -- Signals
    --
    
    --GND mng
    signal sig_GND : std_logic;
    
    -- RTSNoC signals
    signal sig_noc_reset    : std_logic;
        --Ports of the router (clockwise: nn, ne, ee, se, ss, sw, ww, nw) 
    type noc_array_of_stdlogic is array(0 to ROUTER_N_PORTS-1) of std_logic;
    type noc_array_of_stdvec38 is array(0 to ROUTER_N_PORTS-1) of std_logic_vector(NET_BUS_SIZE-1 downto 0);
    signal sig_noc_din   : noc_array_of_stdvec38;
    signal sig_noc_dout  : noc_array_of_stdvec38;
    signal sig_noc_wr    : noc_array_of_stdlogic;
    signal sig_noc_rd    : noc_array_of_stdlogic;
    signal sig_noc_wait  : noc_array_of_stdlogic;
    signal sig_noc_nd    : noc_array_of_stdlogic;

    -- DD3 AXI4 signals
    signal sig_ddr3_awvalid : std_logic;
    signal sig_ddr3_awready : std_logic;
    signal sig_ddr3_awaddr  : std_logic_vector(31 downto 0);
    signal sig_ddr3_awprot  : std_logic_vector(2 downto 0);

    signal sig_ddr3_wvalid  : std_logic;
    signal sig_ddr3_wready  : std_logic;
    signal sig_ddr3_wdata   : std_logic_vector(31 downto 0);
    signal sig_ddr3_wstrb   : std_logic_vector(3 downto 0);

    signal sig_ddr3_bvalid  : std_logic;
    signal sig_ddr3_bready  : std_logic;
    signal sig_ddr3_bresp   : std_logic_vector(1 downto 0);

    signal sig_ddr3_arvalid : std_logic;
    signal sig_ddr3_arready : std_logic;
    signal sig_ddr3_araddr  : std_logic_vector(31 downto 0);
    signal sig_ddr3_arprot  : std_logic_vector(2 downto 0);

    signal sig_ddr3_rvalid  : std_logic;
    signal sig_ddr3_rready  : std_logic;
    signal sig_ddr3_rdata   : std_logic_vector(31 downto 0);
    signal sig_ddr3_rresp   : std_logic_vector(1 downto 0);

    --Debug signals
    --signal sig_chipscope_ctrl       : std_logic_vector(35 downto 0);
    --signal sig_chipscope_data  : std_logic_vector(255 downto 0);
    --signal sig_chipscope_trig0  : std_logic_vector(0 downto 0);
    --signal sig_chipscope_trig1  : std_logic_vector(0 downto 0);
    --signal sig_chipscope_trig2  : std_logic_vector(0 downto 0);
    --signal sig_chipscope_trig3  : std_logic_vector(0 downto 0);
    --signal sig_chipscope_trig4  : std_logic_vector(0 downto 0);
    --signal sig_chipscope_trig5  : std_logic_vector(0 downto 0);
    --signal sig_chipscope_trig6  : std_logic_vector(0 downto 0);
    --signal sig_chipscope_trig7  : std_logic_vector(0 downto 0);
    
    --signal sig_noc_debug_aux    : std_logic_vector(NET_BUS_SIZE_INV-1 downto 0);

begin

    sig_GND <= '0';

    -- -----------------------------------------------------
    -- RTSNoC router
    -- -----------------------------------------------------
    
    rtsnoc_router: ROUTER 
        GENERIC MAP (
            p_X         => conv_integer(ROUTER_X),
            p_Y         => conv_integer(ROUTER_Y),
            p_DATA      => NET_DATA_WIDTH,                    
            p_SIZE_X    => NET_SIZE_X_LOG2,
            p_SIZE_Y    => NET_SIZE_Y_LOG2)
        PORT MAP(
            o_TESTE     => open,
            i_CLK       => clk_i,
            i_RST       => sig_noc_reset,
            -- NORTH
            i_DIN_NN    => sig_noc_din(ROUTER_NN),
            o_DOUT_NN   => sig_noc_dout(ROUTER_NN),
            i_WR_NN     => sig_noc_wr(ROUTER_NN),
            i_RD_NN     => sig_noc_rd(ROUTER_NN),
            o_WAIT_NN   => sig_noc_wait(ROUTER_NN),
            o_ND_NN     => sig_noc_nd(ROUTER_NN),
            -- NORTHEAST
            i_DIN_NE    => sig_noc_din(ROUTER_NE),
            o_DOUT_NE   => sig_noc_dout(ROUTER_NE),
            i_WR_NE     => sig_noc_wr(ROUTER_NE),
            i_RD_NE     => sig_noc_rd(ROUTER_NE),
            o_WAIT_NE   => sig_noc_wait(ROUTER_NE),
            o_ND_NE     => sig_noc_nd(ROUTER_NE),
            -- EAST
            i_DIN_EE    => sig_noc_din(ROUTER_EE),
            o_DOUT_EE   => sig_noc_dout(ROUTER_EE),
            i_WR_EE     => sig_noc_wr(ROUTER_EE),
            i_RD_EE     => sig_noc_rd(ROUTER_EE),
            o_WAIT_EE   => sig_noc_wait(ROUTER_EE),
            o_ND_EE     => sig_noc_nd(ROUTER_EE),
            -- SOUTHEAST
            i_DIN_SE    => sig_noc_din(ROUTER_SE),
            o_DOUT_SE   => sig_noc_dout(ROUTER_SE),
            i_WR_SE     => sig_noc_wr(ROUTER_SE),
            i_RD_SE     => sig_noc_rd(ROUTER_SE),
            o_WAIT_SE   => sig_noc_wait(ROUTER_SE),
            o_ND_SE     => sig_noc_nd(ROUTER_SE),
            -- SOUTH
            i_DIN_SS    => sig_noc_din(ROUTER_SS),
            o_DOUT_SS   => sig_noc_dout(ROUTER_SS),
            i_WR_SS     => sig_noc_wr(ROUTER_SS),
            i_RD_SS     => sig_noc_rd(ROUTER_SS),
            o_WAIT_SS   => sig_noc_wait(ROUTER_SS),
            o_ND_SS     => sig_noc_nd(ROUTER_SS),
            -- SOUTHWEST
            i_DIN_SW    => sig_noc_din(ROUTER_SW),
            o_DOUT_SW   => sig_noc_dout(ROUTER_SW),
            i_WR_SW     => sig_noc_wr(ROUTER_SW),
            i_RD_SW     => sig_noc_rd(ROUTER_SW),
            o_WAIT_SW   => sig_noc_wait(ROUTER_SW),
            o_ND_SW     => sig_noc_nd(ROUTER_SW),
            -- WEST
            i_DIN_WW    => sig_noc_din(ROUTER_WW),
            o_DOUT_WW   => sig_noc_dout(ROUTER_WW),
            i_WR_WW     => sig_noc_wr(ROUTER_WW),
            i_RD_WW     => sig_noc_rd(ROUTER_WW),
            o_WAIT_WW   => sig_noc_wait(ROUTER_WW),
            o_ND_WW     => sig_noc_nd(ROUTER_WW),
            -- NORTHWEST
            i_DIN_NW    => sig_noc_din(ROUTER_NW),
            o_DOUT_NW   => sig_noc_dout(ROUTER_NW),
            i_WR_NW     => sig_noc_wr(ROUTER_NW),
            i_RD_NW     => sig_noc_rd(ROUTER_NW),
            o_WAIT_NW   => sig_noc_wait(ROUTER_NW),
            o_ND_NW     => sig_noc_nd(ROUTER_NW) 
    );
    
    rtsnoc_reset : rtsnoc_axi4lite_reset
        port map (
            axi_rst_i   => reset_i,
            noc_reset_o => sig_noc_reset
        );
     
    -- ------------------------------------------------------------
    -- NODE 0:0:0x000(NN) - Proc node + External RAM (DDR3 controller)
    -- ------------------------------------------------------------
    --     
    proc_node: axi4lite_proc_node
        generic map (
            CLK_FREQ    => CLK_FREQ,
            -- RTSNoC generics
            NET_SIZE_X          => NET_SIZE_X,
            NET_SIZE_Y          => NET_SIZE_Y,
            NET_SIZE_X_LOG2     => NET_SIZE_X_LOG2,
            NET_SIZE_Y_LOG2     => NET_SIZE_Y_LOG2,
            NET_DATA_WIDTH      => NET_DATA_WIDTH,
            NET_BUS_SIZE        => NET_BUS_SIZE,
            ROUTER_X_ADDR       => conv_integer(ROUTER_X),
            ROUTER_Y_ADDR       => conv_integer(ROUTER_Y),
            ROUTER_LOCAL_ADDR   => ROUTER_ADDRS(ROUTER_NN))
        port map(
            -- System signals
            clk_i       => clk_i,
            reset_axi_i   => reset_i,
            reset_noc_i   => sig_noc_reset,
            -- Peripherals
            uart_tx_o   => uart_tx_o,
            uart_rx_i   => uart_rx_i,
            uart_baud_o => uart_baud_o,
            gpio_i      => gpio_i,
            gpio_o      => gpio_o,
            ext_int_i   => ext_int_i,
            -- NoC interface
            noc_din_o   => sig_noc_din(ROUTER_NN),
            noc_dout_i  => sig_noc_dout(ROUTER_NN),
            noc_wr_o    => sig_noc_wr(ROUTER_NN),
            noc_rd_o    => sig_noc_rd(ROUTER_NN),
            noc_wait_i  => sig_noc_wait(ROUTER_NN),
            noc_nd_i    => sig_noc_nd(ROUTER_NN),
            -- EXT RAM
            ext_ram_awaddr_o    => sig_ddr3_awaddr,
            ext_ram_awvalid_o   => sig_ddr3_awvalid,
            ext_ram_awready_i   => sig_ddr3_awready,
            ext_ram_wdata_o     => sig_ddr3_wdata,
            ext_ram_wstrb_o     => sig_ddr3_wstrb,
            ext_ram_wvalid_o    => sig_ddr3_wvalid,
            ext_ram_wready_i    => sig_ddr3_wready,
            ext_ram_bresp_i     => sig_ddr3_bresp,
            ext_ram_bvalid_i    => sig_ddr3_bvalid,
            ext_ram_bready_o    => sig_ddr3_bready,
            ext_ram_araddr_o    => sig_ddr3_araddr,
            ext_ram_arvalid_o   => sig_ddr3_arvalid,
            ext_ram_arready_i   => sig_ddr3_arready,
            ext_ram_rdata_i     => sig_ddr3_rdata,
            ext_ram_rresp_i     => sig_ddr3_rresp,
            ext_ram_rvalid_i    => sig_ddr3_rvalid,
            ext_ram_rready_o    => sig_ddr3_rready
        );
        
    ddr_ctrl: ram_amba_1024k
        port map(
            s_aclk        => clk_i,
            s_aresetn     => reset_i,

            s_axi_awvalid => sig_ddr3_awvalid,
            s_axi_awready => sig_ddr3_awready,
            s_axi_awaddr  => sig_ddr3_awaddr,

            s_axi_wvalid  => sig_ddr3_wvalid,
            s_axi_wready  => sig_ddr3_wready,
            s_axi_wdata   => sig_ddr3_wdata,
            s_axi_wstrb   => sig_ddr3_wstrb, 

            s_axi_bvalid  => sig_ddr3_bvalid,
            s_axi_bready  => sig_ddr3_bready,
            s_axi_bresp   => sig_ddr3_bresp,

            s_axi_arvalid => sig_ddr3_arvalid,
            s_axi_arready => sig_ddr3_arready,
            s_axi_araddr  => sig_ddr3_araddr,

            s_axi_rvalid  => sig_ddr3_rvalid,
            s_axi_rready  => sig_ddr3_rready,
            s_axi_rdata   => sig_ddr3_rdata,
            s_axi_rresp   => sig_ddr3_rresp);

        
    -- ------------------------------------------------------------
    -- NODE 0:0:0x110(WW) - Echo P0
    -- NODE 0:0:0x100(SS) - Echo P1
    -- ------------------------------------------------------------
    
    echo_node : rtsnoc_echo
        generic map (
            SOC_SIZE_X      => NET_SIZE_X_LOG2,
            SOC_SIZE_Y      => NET_SIZE_Y_LOG2,
            NOC_DATA_WIDTH  => NET_DATA_WIDTH,
            P0_ADDR         => NODE_ECHO_P0,
            P1_ADDR         => NODE_ECHO_P1)
        port map(
            -- System signals
            clk_i       => clk_i,
            rst_i       => sig_noc_reset,
            -- NoC signals
            p0_din_o   => sig_noc_din(ROUTER_WW),
            p0_dout_i  => sig_noc_dout(ROUTER_WW),
            p0_wr_o    => sig_noc_wr(ROUTER_WW),
            p0_rd_o    => sig_noc_rd(ROUTER_WW),
            p0_wait_i  => sig_noc_wait(ROUTER_WW),
            p0_nd_i    => sig_noc_nd(ROUTER_WW),
            p1_din_o   => sig_noc_din(ROUTER_SS),
            p1_dout_i  => sig_noc_dout(ROUTER_SS),
            p1_wr_o    => sig_noc_wr(ROUTER_SS),
            p1_rd_o    => sig_noc_rd(ROUTER_SS),
            p1_wait_i  => sig_noc_wait(ROUTER_SS),
            p1_nd_i    => sig_noc_nd(ROUTER_SS)
        );
        
    
    -- -----------------------------------------------------
    -- Chipscope PRO Debugging cores
    -- -----------------------------------------------------
    --db_chipscope_icon : chipscope_icon
    --    port map (
    --        CONTROL0 => sig_chipscope_ctrl
    --    );
    --    
    --db_chipscope_ila : chipscope_ila
    --    port map (
    --        CONTROL => sig_chipscope_ctrl,
    --        CLK     => clk_i,
    --        DATA    => sig_chipscope_data,
    --        TRIG0 => sig_chipscope_trig0,
    --        TRIG1 => sig_chipscope_trig1,
    --        TRIG2 => sig_chipscope_trig2,
    --        TRIG3 => sig_chipscope_trig3,
    --        TRIG4 => sig_chipscope_trig4,
    --        TRIG5 => sig_chipscope_trig5,
    --        TRIG6 => sig_chipscope_trig6,
    --        TRIG7 => sig_chipscope_trig7
    --    );
    --    
    --chipscope_data: for i in 0 to (ROUTER_N_PORTS-1) generate
    --    sig_chipscope_data((i*4)+0) <= sig_noc_wr(i);
    --    sig_chipscope_data((i*4)+1) <= sig_noc_rd(i);
    --    sig_chipscope_data((i*4)+2) <= sig_noc_wait(i);
    --    sig_chipscope_data((i*4)+3) <= sig_noc_nd(i);
    --end generate chipscope_data;
    
    --sig_noc_debug_aux <= (others => sig_GND);
    
    --sig_chipscope_data(63 downto 32) <= sig_noc_debug_aux & sig_noc_din(ROUTER_NN);
    --sig_chipscope_data(95 downto 64) <= sig_noc_debug_aux & sig_noc_dout(ROUTER_NN);
    --sig_chipscope_data(127 downto 96) <= sig_noc_debug_aux & sig_noc_din(ROUTER_WW);
    --sig_chipscope_data(159 downto 128) <= sig_noc_debug_aux & sig_noc_dout(ROUTER_WW);
    --sig_chipscope_data(160) <= sig_GND;
    --sig_chipscope_data(255 downto 161) <= (others => sig_GND);
    
    --sig_chipscope_trig0(0) <= sig_noc_wr(ROUTER_NN);
        
    --sig_chipscope_trig1(0) <= sig_noc_wr(ROUTER_WW);
        
    --sig_chipscope_trig2(0) <= sig_noc_nd(ROUTER_NN);
        
    --sig_chipscope_trig3(0) <= sig_noc_nd(ROUTER_WW);
        
    --sig_chipscope_trig4(0) <= sig_noc_rd(ROUTER_NN);
        
    --sig_chipscope_trig5(0) <= sig_noc_rd(ROUTER_WW);
        
    --sig_chipscope_trig6(0) <= sig_noc_wait(ROUTER_NN);
        
    --sig_chipscope_trig7(0) <= sig_GND;
    
end RTL;
