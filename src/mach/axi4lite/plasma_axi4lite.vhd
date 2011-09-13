library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.log2;
use ieee.math_real.ceil;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- Memory Map:
--   0x00000000 - 0x0fffffff   Internal RAM (4KB - 256MB)
--   0x10000000 - 0x1fffffff   External RAM (1MB - 256MB)
--   All peripherals have a 1 KB address space
--   0x80000000  Uart
--   0x80000400  GPIO
--   0x80000800  Timer
--   0x80000C00  PIC
--   0x80001000  RTSNoC

entity plasma_axi4lite is
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

end plasma_axi4lite;

architecture RTL of plasma_axi4lite is
    -- AXI4 constants
    constant N_SLAVES       : integer := 7;
    constant N_SLAVES_LOG   : integer := integer(ceil(log2(real(N_SLAVES))));

    constant SLAVE0_ADDR_W  : integer := 4;
    constant SLAVE0_ADDR    : std_logic_vector(SLAVE0_ADDR_W-1 downto 0) := b"0000";

    constant SLAVE1_ADDR_W  : integer := 4;
    constant SLAVE1_ADDR    : std_logic_vector(SLAVE1_ADDR_W-1 downto 0) := b"0001";

    constant SLAVE2_ADDR_W  : integer := 22;
    constant SLAVE2_ADDR    : std_logic_vector(SLAVE2_ADDR_W-1 downto 0) 
        := b"1000" & b"0000" & b"0000" & b"0000" & b"0000" & b"00";
        
    constant SLAVE3_ADDR_W  : integer := 22;
    constant SLAVE3_ADDR    : std_logic_vector(SLAVE3_ADDR_W-1 downto 0) 
        := b"1000" & b"0000" & b"0000" & b"0000" & b"0000" & b"01";
        
    constant SLAVE4_ADDR_W  : integer := 22;
    constant SLAVE4_ADDR    : std_logic_vector(SLAVE4_ADDR_W-1 downto 0) 
        := b"1000" & b"0000" & b"0000" & b"0000" & b"0000" & b"10";
        
    constant SLAVE5_ADDR_W  : integer := 22;
    constant SLAVE5_ADDR    : std_logic_vector(SLAVE5_ADDR_W-1 downto 0) 
        := b"1000" & b"0000" & b"0000" & b"0000" & b"0000" & b"11";
        
    constant SLAVE6_ADDR_W  : integer := 22;
    constant SLAVE6_ADDR    : std_logic_vector(SLAVE6_ADDR_W-1 downto 0) 
        := b"1000" & b"0000" & b"0000" & b"0000" & b"0001" & b"00";

    -- RTSNoC constants
        -- Dimension of the network and router address.
    constant NET_SIZE_X         : integer := 1;
    constant NET_SIZE_Y         : integer := 1;
    constant NET_SIZE_X_LOG2    : integer := 1;
    constant NET_SIZE_Y_LOG2    : integer := 1;
    constant ROUTER_X           : std_logic_vector(NET_SIZE_X_LOG2-1 downto 0) := "0";
    constant ROUTER_Y           : std_logic_vector(NET_SIZE_X_LOG2-1 downto 0) := "0";
    constant NET_DATA_WIDTH     : integer := 16;
        -- Local addresses
    constant ROUTER_NN  : std_logic_vector(2 downto 0) := "000";  
    constant ROUTER_NE  : std_logic_vector(2 downto 0) := "001";
    constant ROUTER_EE  : std_logic_vector(2 downto 0) := "010";
    constant ROUTER_SE  : std_logic_vector(2 downto 0) := "011";
    constant ROUTER_SS  : std_logic_vector(2 downto 0) := "100";
    constant ROUTER_SW  : std_logic_vector(2 downto 0) := "101";
    constant ROUTER_WW  : std_logic_vector(2 downto 0) := "110";
    constant ROUTER_NW  : std_logic_vector(2 downto 0) := "111";
        -- NoC node addressess
    constant NODE_AXI_ADDR  : std_logic_vector(2 downto 0) := ROUTER_NN;
    constant NODE_ECHO_P0   : std_logic_vector(2 downto 0) := ROUTER_NE;
    constant NODE_ECHO_P1   : std_logic_vector(2 downto 0) := ROUTER_EE;
    
    
    -- 
    -- Declarations
    -- 
    component axi4lite_decoder is
        generic(
            sw          : integer;
            sw_log2     : integer;
            s0_addr_w   : integer; 
            s0_addr     : std_logic_vector(SLAVE0_ADDR_W-1 downto 0);
            s1_addr_w   : integer; 
            s1_addr     : std_logic_vector(SLAVE1_ADDR_W-1 downto 0);
            s2_addr_w   : integer; 
            s2_addr     : std_logic_vector(SLAVE2_ADDR_W-1 downto 0);
            s3_addr_w   : integer; 
            s3_addr     : std_logic_vector(SLAVE3_ADDR_W-1 downto 0);
            s4_addr_w   : integer; 
            s4_addr     : std_logic_vector(SLAVE4_ADDR_W-1 downto 0);
            s5_addr_w   : integer; 
            s5_addr     : std_logic_vector(SLAVE5_ADDR_W-1 downto 0);
            s6_addr_w   : integer; 
            s6_addr     : std_logic_vector(SLAVE6_ADDR_W-1 downto 0));
        port(
            -- master write address channel
            m_awvalid_i    : in std_logic;
            m_awready_o    : out std_logic;
            m_awaddr_i     : in std_logic_vector(31 downto 0);
            m_awprot_i     : in std_logic_vector(2 downto 0);
            -- master write data channel
            m_wvalid_i     : in std_logic;
            m_wready_o     : out std_logic;
            m_wdata_i      : in std_logic_vector(31 downto 0);
            m_wstrb_i      : in std_logic_vector(3 downto 0);
            -- master write response channel
            m_bvalid_o     : out std_logic;
            m_bready_i     : in std_logic;
            m_bresp_o      : out std_logic_vector(1 downto 0);
            -- master read address channel
            m_arvalid_i    : in std_logic;
            m_arready_o    : out std_logic;
            m_araddr_i     : in std_logic_vector(31 downto 0);
            m_arprot_i     : in std_logic_vector(2 downto 0);
            -- master read data channel
            m_rvalid_o     : out std_logic;
            m_rready_i     : in std_logic;
            m_rdata_o      : out std_logic_vector(31 downto 0);
            m_rresp_o      : out std_logic_vector(1 downto 0);
            -- slaves write address channel
            s_awvalid_o    : out std_logic_vector(N_SLAVES-1 downto 0);
            s_awready_i    : in std_logic_vector(N_SLAVES-1 downto 0);
            s_awaddr_o     : out std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_awprot_o     : out std_logic_vector((N_SLAVES*3)-1 downto 0);
            -- slaves write data channel
            s_wvalid_o     : out std_logic_vector(N_SLAVES-1 downto 0);
            s_wready_i     : in std_logic_vector(N_SLAVES-1 downto 0);
            s_wdata_o      : out std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_wstrb_o      : out std_logic_vector((N_SLAVES*4)-1 downto 0);
            -- slaves write response channel
            s_bvalid_i     : in std_logic_vector(N_SLAVES-1 downto 0);
            s_bready_o     : out std_logic_vector(N_SLAVES-1 downto 0);
            s_bresp_i      : in std_logic_vector((N_SLAVES*2)-1 downto 0);
            -- slaves read address channel
            s_arvalid_o    : out std_logic_vector(N_SLAVES-1 downto 0);
            s_arready_i    : in std_logic_vector(N_SLAVES-1 downto 0);
            s_araddr_o     : out std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_arprot_o     : out std_logic_vector((N_SLAVES*3)-1 downto 0);
            -- slaves read data channel
            s_rvalid_i     : in std_logic_vector(N_SLAVES-1 downto 0);
            s_rready_o     : out std_logic_vector(N_SLAVES-1 downto 0);
            s_rdata_i      : in std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_rresp_i      : in std_logic_vector((N_SLAVES*2)-1 downto 0));
    end component axi4lite_decoder;

    component plasma_axi4lite_master is
        generic(
            memory_type     : string  := "XILINX_16X";
            mult_type       : string  := "DEFAULT";
            shifter_type    : string  := "DEFAULT";
            alu_type        : string  := "DEFAULT";
            pipeline_stages : natural := 2); --2 or 3
        port(
            aclk       : in std_logic;
            areset     : in std_logic;
            -- write address channel
            awvalid    : out std_logic;
            awready    : in std_logic;
            awaddr     : out std_logic_vector(31 downto 0);
            awprot     : out std_logic_vector(2 downto 0);
            -- write data channel
            wvalid     : out std_logic;
            wready     : in std_logic;
            wdata      : out std_logic_vector(31 downto 0);
            wstrb      : out std_logic_vector(3 downto 0);
            -- write response channel
            bvalid     : in std_logic;
            bready     : out std_logic;
            bresp      : in std_logic_vector(1 downto 0);
            -- read address channel
            arvalid    : out std_logic;
            arready    : in std_logic;
            araddr     : out std_logic_vector(31 downto 0);
            arprot     : out std_logic_vector(2 downto 0);
            -- read data channel
            rvalid     : in std_logic;
            rready     : out std_logic;
            rdata      : in std_logic_vector(31 downto 0);
            rresp      : in std_logic_vector(1 downto 0);

            -- plasma cpu interrupt, externalized
            intr       : in std_logic);
    end component;

    component ram_amba_128k is
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

    component simple_uart_axi4lite is
        generic(
            TXDEPTH : integer;
            RXDEPTH : integer);
        port(
            -- System signals
            clk_i       : in  std_logic;
            axi_rst_i   : in  std_logic;
            rx_int_o    : out std_logic;
            tx_int_o    : out std_logic;
            -- AXI signals
            axi_awaddr_i          : in  std_logic_vector(31 downto 0);
            axi_awvalid_i         : in  std_logic;
            axi_awready_o         : out std_logic;
            axi_wdata_i           : in  std_logic_vector(31 downto 0);
            axi_wstrb_i           : in  std_logic_vector(3 downto 0);
            axi_wvalid_i          : in  std_logic;
            axi_wready_o          : out std_logic;
            axi_bresp_o           : out std_logic_vector(1 downto 0);
            axi_bvalid_o          : out std_logic;
            axi_bready_i          : in  std_logic;
            axi_araddr_i          : in  std_logic_vector(31 downto 0);
            axi_arvalid_i         : in  std_logic;
            axi_arready_o         : out std_logic;
            axi_rdata_o           : out std_logic_vector(31 downto 0);
            axi_rresp_o           : out std_logic_vector(1 downto 0);
            axi_rvalid_o          : out std_logic;
            axi_rready_i          : in  std_logic;
            -- UARTLite Interface Signals
            rx_i    : in  std_logic;
            tx_o    : out std_logic;
            baud_o  : out std_logic);
    end component;
    
    component gpio_axi4lite is
        port(
            -- AXI signals
            clk_i       : in  std_logic;
            axi_rst_i   : in  std_logic;
            axi_awaddr_i          : in  std_logic_vector(31 downto 0);
            axi_awvalid_i         : in  std_logic;
            axi_awready_o         : out std_logic;
            axi_wdata_i           : in  std_logic_vector(31 downto 0);
            axi_wstrb_i           : in  std_logic_vector(3 downto 0);
            axi_wvalid_i          : in  std_logic;
            axi_wready_o          : out std_logic;
            axi_bresp_o           : out std_logic_vector(1 downto 0);
            axi_bvalid_o          : out std_logic;
            axi_bready_i          : in  std_logic;
            axi_araddr_i          : in  std_logic_vector(31 downto 0);
            axi_arvalid_i         : in  std_logic;
            axi_arready_o         : out std_logic;
            axi_rdata_o           : out std_logic_vector(31 downto 0);
            axi_rresp_o           : out std_logic_vector(1 downto 0);
            axi_rvalid_o          : out std_logic;
            axi_rready_i          : in  std_logic;
            -- GPIO
            gpio_i  : in  std_logic_vector(31 downto 0);
            gpio_o  : out  std_logic_vector(31 downto 0));
    end component;

    component timer_axi4lite is
        port(
            -- AXI signals
            clk_i       : in  std_logic;
            axi_rst_i   : in  std_logic;
            axi_awaddr_i          : in  std_logic_vector(31 downto 0);
            axi_awvalid_i         : in  std_logic;
            axi_awready_o         : out std_logic;
            axi_wdata_i           : in  std_logic_vector(31 downto 0);
            axi_wstrb_i           : in  std_logic_vector(3 downto 0);
            axi_wvalid_i          : in  std_logic;
            axi_wready_o          : out std_logic;
            axi_bresp_o           : out std_logic_vector(1 downto 0);
            axi_bvalid_o          : out std_logic;
            axi_bready_i          : in  std_logic;
            axi_araddr_i          : in  std_logic_vector(31 downto 0);
            axi_arvalid_i         : in  std_logic;
            axi_arready_o         : out std_logic;
            axi_rdata_o           : out std_logic_vector(31 downto 0);
            axi_rresp_o           : out std_logic_vector(1 downto 0);
            axi_rvalid_o          : out std_logic;
            axi_rready_i          : in  std_logic;
            -- Int
            int_o  : out  std_logic);
    end component;
    
    component pic_axi4lite is
        port(
            -- AXI signals
            clk_i       : in  std_logic;
            axi_rst_i   : in  std_logic;
            axi_awaddr_i          : in  std_logic_vector(31 downto 0);
            axi_awvalid_i         : in  std_logic;
            axi_awready_o         : out std_logic;
            axi_wdata_i           : in  std_logic_vector(31 downto 0);
            axi_wstrb_i           : in  std_logic_vector(3 downto 0);
            axi_wvalid_i          : in  std_logic;
            axi_wready_o          : out std_logic;
            axi_bresp_o           : out std_logic_vector(1 downto 0);
            axi_bvalid_o          : out std_logic;
            axi_bready_i          : in  std_logic;
            axi_araddr_i          : in  std_logic_vector(31 downto 0);
            axi_arvalid_i         : in  std_logic;
            axi_arready_o         : out std_logic;
            axi_rdata_o           : out std_logic_vector(31 downto 0);
            axi_rresp_o           : out std_logic_vector(1 downto 0);
            axi_rvalid_o          : out std_logic;
            axi_rready_i          : in  std_logic;
            -- Int
            irq_i  : in  std_logic_vector(31 downto 0);
            int_o  : out  std_logic);
    end component;

    COMPONENT ROUTER
        GENERIC (
            p_X         : integer := conv_integer(ROUTER_X);
            p_Y         : integer := conv_integer(ROUTER_Y);
            p_DATA      : integer := NET_DATA_WIDTH;                    
            p_SIZE_X    : integer := NET_SIZE_X;
            p_SIZE_Y    : integer := NET_SIZE_Y);  
        PORT(
            i_CLK       : IN std_logic;
            i_RST       : IN std_logic;
            i_DIN_NN    : IN std_logic_vector(37 downto 0);
            i_WR_NN     : IN std_logic;
            i_RD_NN     : IN std_logic;
            i_DIN_NE    : IN std_logic_vector(37 downto 0);
            i_WR_NE     : IN std_logic;
            i_RD_NE     : IN std_logic;
            i_DIN_EE    : IN std_logic_vector(37 downto 0);
            i_WR_EE     : IN std_logic;
            i_RD_EE     : IN std_logic;
            i_DIN_SE    : IN std_logic_vector(37 downto 0);
            i_WR_SE     : IN std_logic;
            i_RD_SE     : IN std_logic;
            i_DIN_SS    : IN std_logic_vector(37 downto 0);
            i_WR_SS     : IN std_logic;
            i_RD_SS     : IN std_logic;
            i_DIN_SW    : IN std_logic_vector(37 downto 0);
            i_WR_SW     : IN std_logic;
            i_RD_SW     : IN std_logic;
            i_DIN_WW    : IN std_logic_vector(37 downto 0);
            i_WR_WW     : IN std_logic;
            i_RD_WW     : IN std_logic;
            i_DIN_NW    : IN std_logic_vector(37 downto 0);
            i_WR_NW     : IN std_logic;
            i_RD_NW     : IN std_logic;          
            o_DOUT_NN   : OUT std_logic_vector(37 downto 0);
            o_WAIT_NN   : OUT std_logic;
            o_ND_NN     : OUT std_logic;
            o_DOUT_NE   : OUT std_logic_vector(37 downto 0);
            o_WAIT_NE   : OUT std_logic;
            o_ND_NE     : OUT std_logic;
            o_DOUT_EE   : OUT std_logic_vector(37 downto 0);
            o_WAIT_EE   : OUT std_logic;
            o_ND_EE     : OUT std_logic;
            o_DOUT_SE   : OUT std_logic_vector(37 downto 0);
            o_WAIT_SE   : OUT std_logic;
            o_ND_SE     : OUT std_logic;
            o_DOUT_SS   : OUT std_logic_vector(37 downto 0);
            o_WAIT_SS   : OUT std_logic;
            o_ND_SS     : OUT std_logic;
            o_DOUT_SW   : OUT std_logic_vector(37 downto 0);
            o_WAIT_SW   : OUT std_logic;
            o_ND_SW     : OUT std_logic;
            o_DOUT_WW   : OUT std_logic_vector(37 downto 0);
            o_WAIT_WW   : OUT std_logic;
            o_ND_WW     : OUT std_logic;
            o_DOUT_NW   : OUT std_logic_vector(37 downto 0);
            o_WAIT_NW   : OUT std_logic;
            o_ND_NW     : OUT std_logic
        );
    END COMPONENT;
    
    component rtsnoc_axi4lite_slave is
        generic (
            NOC_LOCAL_ADR   : std_logic_vector(2 downto 0) := NODE_AXI_ADDR; 
            NOC_X           : std_logic_vector(NET_SIZE_X_LOG2-1 downto 0) := ROUTER_X;
            NOC_Y           : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := ROUTER_Y;
            SOC_SIZE_X      : integer := NET_SIZE_X_LOG2;
            SOC_SIZE_Y      : integer := NET_SIZE_Y_LOG2;
            NOC_DATA_WIDTH  : integer := NET_DATA_WIDTH);
        port(
            -- AXI signals
            clk_i       : in  std_logic;
            axi_rst_i   : in  std_logic;
            axi_awaddr_i          : in  std_logic_vector(31 downto 0);
            axi_awvalid_i         : in  std_logic;
            axi_awready_o         : out std_logic;
            axi_wdata_i           : in  std_logic_vector(31 downto 0);
            axi_wstrb_i           : in  std_logic_vector(3 downto 0);
            axi_wvalid_i          : in  std_logic;
            axi_wready_o          : out std_logic;
            axi_bresp_o           : out std_logic_vector(1 downto 0);
            axi_bvalid_o          : out std_logic;
            axi_bready_i          : in  std_logic;
            axi_araddr_i          : in  std_logic_vector(31 downto 0);
            axi_arvalid_i         : in  std_logic;
            axi_arready_o         : out std_logic;
            axi_rdata_o           : out std_logic_vector(31 downto 0);
            axi_rresp_o           : out std_logic_vector(1 downto 0);
            axi_rvalid_o          : out std_logic;
            axi_rready_i          : in  std_logic;
            -- NoC signals
            noc_reset_o : out std_logic;
            noc_din_o   : out std_logic_vector(37 downto 0);
            noc_dout_i  : in std_logic_vector(37 downto 0);
            noc_wr_o    : out std_logic;
            noc_rd_o    : out std_logic;
            noc_wait_i  : in std_logic;
            noc_nd_i    : in std_logic;
            noc_int_o   : out  std_logic);
    end component;
    
    component rtsnoc_echo is
        generic (
            P0_ADDR          : std_logic_vector(2 downto 0) := NODE_ECHO_P0; 
            P0_ADDR_X        : std_logic_vector(NET_SIZE_X_LOG2-1 downto 0) := ROUTER_X;
            P0_ADDR_Y        : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := ROUTER_Y;
            P1_ADDR          : std_logic_vector(2 downto 0) := NODE_ECHO_P1; 
            P1_ADDR_X        : std_logic_vector(NET_SIZE_X_LOG2-1 downto 0) := ROUTER_X;
            P1_ADDR_Y        : std_logic_vector(NET_SIZE_Y_LOG2-1 downto 0) := ROUTER_Y;
            SOC_SIZE_X      : integer := NET_SIZE_X_LOG2;
            SOC_SIZE_Y      : integer := NET_SIZE_Y_LOG2;
            NOC_DATA_WIDTH  : integer := NET_DATA_WIDTH);
        port(
            clk_i      : in std_logic;
            rst_i      : in std_logic;
            p0_din_o   : out std_logic_vector(37 downto 0);
            p0_dout_i  : in std_logic_vector(37 downto 0);
            p0_wr_o    : out std_logic;
            p0_rd_o    : out std_logic;
            p0_wait_i  : in std_logic;
            p0_nd_i    : in std_logic;
            p1_din_o   : out std_logic_vector(37 downto 0);
            p1_dout_i  : in std_logic_vector(37 downto 0);
            p1_wr_o    : out std_logic;
            p1_rd_o    : out std_logic;
            p1_wait_i  : in std_logic;
            p1_nd_i    : in std_logic);
    end component;


    --  
    -- Signals
    -- 

    -- AMBA AXI4 signals
    -- Master
    signal sig_m0_awvalid : std_logic;
    signal sig_m0_awready : std_logic;
    signal sig_m0_awaddr  : std_logic_vector(31 downto 0);
    signal sig_m0_awprot  : std_logic_vector(2 downto 0);

    signal sig_m0_wvalid  : std_logic;
    signal sig_m0_wready  : std_logic;
    signal sig_m0_wdata   : std_logic_vector(31 downto 0);
    signal sig_m0_wstrb   : std_logic_vector(3 downto 0);

    signal sig_m0_bvalid  : std_logic;
    signal sig_m0_bready  : std_logic;
    signal sig_m0_bresp   : std_logic_vector(1 downto 0);

    signal sig_m0_arvalid : std_logic;
    signal sig_m0_arready : std_logic;
    signal sig_m0_araddr  : std_logic_vector(31 downto 0);
    signal sig_m0_arprot  : std_logic_vector(2 downto 0);

    signal sig_m0_rvalid  : std_logic;
    signal sig_m0_rready  : std_logic;
    signal sig_m0_rdata   : std_logic_vector(31 downto 0);
    signal sig_m0_rresp   : std_logic_vector(1 downto 0);


    -- AMBA AXI4 signals
    -- Slaves
    signal sig_awvalid  : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_awready  : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_awaddr   : std_logic_vector((N_SLAVES*32)-1 downto 0);
    signal sig_awprot   : std_logic_vector((N_SLAVES*3)-1 downto 0);

    signal sig_wvalid   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_wready   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_wdata    : std_logic_vector((N_SLAVES*32)-1 downto 0);
    signal sig_wstrb    : std_logic_vector((N_SLAVES*4)-1 downto 0);

    signal sig_bvalid   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_bready   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_bresp    : std_logic_vector((N_SLAVES*2)-1 downto 0);

    signal sig_arvalid  : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_arready  : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_araddr   : std_logic_vector((N_SLAVES*32)-1 downto 0);
    signal sig_arprot   : std_logic_vector((N_SLAVES*3)-1 downto 0);

    signal sig_rvalid   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_rready   : std_logic_vector(N_SLAVES-1 downto 0);
    signal sig_rdata    : std_logic_vector((N_SLAVES*32)-1 downto 0);
    signal sig_rresp    : std_logic_vector((N_SLAVES*2)-1 downto 0);

    type axi_array_of_stdlogic is array(0 to N_SLAVES-1) of std_logic;
    type axi_array_of_stdvec2 is array(0 to N_SLAVES-1) of std_logic_vector(1 downto 0);
    type axi_array_of_stdvec3 is array(0 to N_SLAVES-1) of std_logic_vector(2 downto 0);
    type axi_array_of_stdvec4 is array(0 to N_SLAVES-1) of std_logic_vector(3 downto 0);
    type axi_array_of_stdvec32 is array(0 to N_SLAVES-1) of std_logic_vector(31 downto 0);

    signal sig_slaves_awvalid : axi_array_of_stdlogic;
    signal sig_slaves_awready : axi_array_of_stdlogic;
    signal sig_slaves_awaddr  : axi_array_of_stdvec32;
    signal sig_slaves_awprot  : axi_array_of_stdvec3;

    signal sig_slaves_wvalid  : axi_array_of_stdlogic;
    signal sig_slaves_wready  : axi_array_of_stdlogic;
    signal sig_slaves_wdata   : axi_array_of_stdvec32;
    signal sig_slaves_wstrb   : axi_array_of_stdvec4;

    signal sig_slaves_bready  : axi_array_of_stdlogic;
    signal sig_slaves_bvalid  : axi_array_of_stdlogic;
    signal sig_slaves_bresp   : axi_array_of_stdvec2;

    signal sig_slaves_arvalid : axi_array_of_stdlogic;
    signal sig_slaves_arready : axi_array_of_stdlogic;
    signal sig_slaves_araddr  : axi_array_of_stdvec32;
    signal sig_slaves_arprot  : axi_array_of_stdvec3;

    signal sig_slaves_rready  : axi_array_of_stdlogic;
    signal sig_slaves_rdata   : axi_array_of_stdvec32;
    signal sig_slaves_rresp   : axi_array_of_stdvec2;
    signal sig_slaves_rvalid  : axi_array_of_stdlogic;
    
    
    signal sig_irqs         : std_logic_vector(31 downto 0);
    signal sig_int_timer    : std_logic;
    signal sig_uart_rx_int  : std_logic;
    signal sig_uart_tx_int  : std_logic;
    
    -- RTSNoC signals
    signal sig_noc_reset    : std_logic;
        --Ports 
    signal sig_noc_nn_din   : std_logic_vector(37 downto 0);
    signal sig_noc_nn_dout  : std_logic_vector(37 downto 0);
    signal sig_noc_nn_wr    : std_logic;
    signal sig_noc_nn_rd    : std_logic;
    signal sig_noc_nn_wait  : std_logic;
    signal sig_noc_nn_nd    : std_logic;
    signal sig_noc_ne_din   : std_logic_vector(37 downto 0);
    signal sig_noc_ne_dout  : std_logic_vector(37 downto 0);
    signal sig_noc_ne_wr    : std_logic;
    signal sig_noc_ne_rd    : std_logic;
    signal sig_noc_ne_wait  : std_logic;
    signal sig_noc_ne_nd    : std_logic;
    signal sig_noc_ee_din   : std_logic_vector(37 downto 0);
    signal sig_noc_ee_dout  : std_logic_vector(37 downto 0);
    signal sig_noc_ee_wr    : std_logic;
    signal sig_noc_ee_rd    : std_logic;
    signal sig_noc_ee_wait  : std_logic;
    signal sig_noc_ee_nd    : std_logic;
        -- AXI port interrupt
    signal sig_noc_int    : std_logic;
    
    -- Plasma signals
    signal sig_intr    : std_logic;

begin

    -- -----------------------------------------------------
    -- AMBA interconnect
    -- -----------------------------------------------------
    
    axi4lite_interconnect: axi4lite_decoder
        generic map(
            sw          => N_SLAVES,
            sw_log2     => N_SLAVES_LOG,
            s0_addr_w   => SLAVE0_ADDR_W, 
            s0_addr     => SLAVE0_ADDR,
            s1_addr_w   => SLAVE1_ADDR_W, 
            s1_addr     => SLAVE1_ADDR,
            s2_addr_w   => SLAVE2_ADDR_W, 
            s2_addr     => SLAVE2_ADDR,
            s3_addr_w   => SLAVE3_ADDR_W, 
            s3_addr     => SLAVE3_ADDR,
            s4_addr_w   => SLAVE4_ADDR_W, 
            s4_addr     => SLAVE4_ADDR,
            s5_addr_w   => SLAVE5_ADDR_W, 
            s5_addr     => SLAVE5_ADDR,
            s6_addr_w   => SLAVE6_ADDR_W, 
            s6_addr     => SLAVE6_ADDR)
        port map(
            -- master write address channel
            m_awvalid_i => sig_m0_awvalid,
            m_awready_o => sig_m0_awready,
            m_awaddr_i  => sig_m0_awaddr,
            m_awprot_i  => sig_m0_awprot,
            -- master write data channel
            m_wvalid_i  => sig_m0_wvalid,
            m_wready_o  => sig_m0_wready,
            m_wdata_i   => sig_m0_wdata,
            m_wstrb_i   => sig_m0_wstrb,
            -- master write response channel
            m_bvalid_o  => sig_m0_bvalid,
            m_bready_i  => sig_m0_bready,
            m_bresp_o   => sig_m0_bresp,
            -- master read address channel
            m_arvalid_i => sig_m0_arvalid,
            m_arready_o => sig_m0_arready,
            m_araddr_i  => sig_m0_araddr,
            m_arprot_i  => sig_m0_arprot,
            -- master read data channel
            m_rvalid_o  => sig_m0_rvalid,
            m_rready_i  => sig_m0_rready,
            m_rdata_o   => sig_m0_rdata,
            m_rresp_o   => sig_m0_rresp,
            -- slaves write address channel
            s_awvalid_o =>  sig_awvalid,
            s_awready_i =>  sig_awready,
            s_awaddr_o  =>  sig_awaddr,
            s_awprot_o  =>  sig_awprot,
            -- slaves write data channel
            s_wvalid_o  =>  sig_wvalid,
            s_wready_i  =>  sig_wready,
            s_wdata_o   =>  sig_wdata,
            s_wstrb_o   =>  sig_wstrb,
            -- slaves write response channel
            s_bvalid_i  =>  sig_bvalid,
            s_bready_o  =>  sig_bready,
            s_bresp_i   =>  sig_bresp,
            -- slaves read address channel
            s_arvalid_o =>  sig_arvalid,
            s_arready_i =>  sig_arready,
            s_araddr_o  =>  sig_araddr,
            s_arprot_o  =>  sig_arprot,
            -- slaves read data channel
            s_rvalid_i  =>  sig_rvalid,
            s_rready_o  =>  sig_rready,
            s_rdata_i   =>  sig_rdata,
            s_rresp_i   =>  sig_rresp);


    slaves_outputs_to_interconnect: for s in 0 to (N_SLAVES-1) generate
        sig_awready(s) <= sig_slaves_awready(s);
        sig_wready(s)  <= sig_slaves_wready(s);
        sig_bvalid(s)  <= sig_slaves_bvalid(s);
        sig_arready(s) <= sig_slaves_arready(s);
        sig_rvalid(s)  <= sig_slaves_rvalid(s);
        sig_bresp( ((s*2)+1) downto (s*2) )    <= sig_slaves_bresp(s);
        sig_rdata( ((s*32)+31) downto (s*32) ) <= sig_slaves_rdata(s);
        sig_rresp( ((s*2)+1) downto (s*2) )    <= sig_slaves_rresp(s);
    end generate slaves_outputs_to_interconnect;

    slaves_inputs_from_interconnect: for s in 0 to (N_SLAVES-1) generate
        sig_slaves_awvalid(s) <= sig_awvalid(s);
        sig_slaves_awaddr(s)  <= sig_awaddr( ((s*32)+31) downto (s*32) );
        sig_slaves_awprot(s)  <= sig_awprot( ((s*3)+2) downto (s*3) );
        sig_slaves_wvalid(s)  <= sig_wvalid(s);
        sig_slaves_wdata(s)   <= sig_wdata( ((s*32)+31) downto (s*32) );
        sig_slaves_wstrb(s)   <= sig_wstrb( ((s*4)+3) downto (s*4) );
        sig_slaves_bready(s)  <= sig_bready(s);
        sig_slaves_arvalid(s) <= sig_arvalid(s);
        sig_slaves_araddr(s)  <= sig_araddr( ((s*32)+31) downto (s*32) );
        sig_slaves_arprot(s)  <= sig_arprot( ((s*3)+2) downto (s*3) );
        sig_slaves_rready(s)  <= sig_rready(s);
    end generate slaves_inputs_from_interconnect;

    -- -----------------------------------------------------
    -- Master - Plasma
    -- -----------------------------------------------------

    plasma_amba: plasma_axi4lite_master
        generic map(
            memory_type     => "XILINX_16X",
            mult_type       => "DEFAULT",
            shifter_type    => "DEFAULT",
            alu_type        => "DEFAULT",
            pipeline_stages => 2)
        port map(
            aclk       => clk_i,
            areset     => reset_i,
            -- write address channel
            awvalid    => sig_m0_awvalid,
            awready    => sig_m0_awready,
            awaddr     => sig_m0_awaddr,
            awprot     => sig_m0_awprot,
            -- write data channel
            wvalid     => sig_m0_wvalid,
            wready     => sig_m0_wready,
            wdata      => sig_m0_wdata,
            wstrb      => sig_m0_wstrb,
            -- write response channel
            bvalid     => sig_m0_bvalid,
            bready     => sig_m0_bready,
            bresp      => sig_m0_bresp,
            -- read address channel
            arvalid    => sig_m0_arvalid,
            arready    => sig_m0_arready,
            araddr     => sig_m0_araddr,
            arprot     => sig_m0_arprot,
            -- read data channel
            rvalid     => sig_m0_rvalid,
            rready     => sig_m0_rready,
            rdata      => sig_m0_rdata,
            rresp      => sig_m0_rresp,

            intr       => sig_intr);
            
    -- -----------------------------------------------------
    -- Slave0 - Internal RAM
    -- -----------------------------------------------------

    ram_amba0: ram_amba_128k
        port map(
            s_aclk        => clk_i,
            s_aresetn     => reset_i,

            s_axi_awvalid => sig_slaves_awvalid(0),
            s_axi_awready => sig_slaves_awready(0),
            s_axi_awaddr  => sig_slaves_awaddr(0),

            s_axi_wvalid  => sig_slaves_wvalid(0),
            s_axi_wready  => sig_slaves_wready(0),
            s_axi_wdata   => sig_slaves_wdata(0),
            s_axi_wstrb   => sig_slaves_wstrb(0), 

            s_axi_bvalid  => sig_slaves_bvalid(0),
            s_axi_bready  => sig_slaves_bready(0),
            s_axi_bresp   => sig_slaves_bresp(0),

            s_axi_arvalid => sig_slaves_arvalid(0),
            s_axi_arready => sig_slaves_arready(0),
            s_axi_araddr  => sig_slaves_araddr(0),

            s_axi_rvalid  => sig_slaves_rvalid(0),
            s_axi_rready  => sig_slaves_rready(0),
            s_axi_rdata   => sig_slaves_rdata(0),
            s_axi_rresp   => sig_slaves_rresp(0));

    -- -----------------------------------------------------
    -- Slave1 - External RAM (DDR3 controller)
    -- -----------------------------------------------------

    ram_amba1: ram_amba_1024k
        port map(
            s_aclk        => clk_i,
            s_aresetn     => reset_i,

            s_axi_awvalid => sig_slaves_awvalid(1),
            s_axi_awready => sig_slaves_awready(1),
            s_axi_awaddr  => sig_slaves_awaddr(1),

            s_axi_wvalid  => sig_slaves_wvalid(1),
            s_axi_wready  => sig_slaves_wready(1),
            s_axi_wdata   => sig_slaves_wdata(1),
            s_axi_wstrb   => sig_slaves_wstrb(1), 

            s_axi_bvalid  => sig_slaves_bvalid(1),
            s_axi_bready  => sig_slaves_bready(1),
            s_axi_bresp   => sig_slaves_bresp(1),

            s_axi_arvalid => sig_slaves_arvalid(1),
            s_axi_arready => sig_slaves_arready(1),
            s_axi_araddr  => sig_slaves_araddr(1),

            s_axi_rvalid  => sig_slaves_rvalid(1),
            s_axi_rready  => sig_slaves_rready(1),
            s_axi_rdata   => sig_slaves_rdata(1),
            s_axi_rresp   => sig_slaves_rresp(1));

    -- -----------------------------------------------------
    -- Slave2 - UART
    -- -----------------------------------------------------

    uart: simple_uart_axi4lite 
        generic map(
            TXDEPTH => 3,
            RXDEPTH => 3)
        port map(
            -- System signals
            clk_i       => clk_i,
            axi_rst_i   => reset_i,
            rx_int_o    => sig_uart_rx_int,
            tx_int_o    => sig_uart_tx_int,
            -- AXI signals
            axi_awaddr_i    => sig_slaves_awaddr(2),
            axi_awvalid_i   => sig_slaves_awvalid(2),
            axi_awready_o   => sig_slaves_awready(2),
            axi_wdata_i     => sig_slaves_wdata(2),
            axi_wstrb_i     => sig_slaves_wstrb(2),
            axi_wvalid_i    => sig_slaves_wvalid(2),
            axi_wready_o    => sig_slaves_wready(2),
            axi_bresp_o     => sig_slaves_bresp(2),
            axi_bvalid_o    => sig_slaves_bvalid(2),
            axi_bready_i    => sig_slaves_bready(2),
            axi_araddr_i    => sig_slaves_araddr(2),
            axi_arvalid_i   => sig_slaves_arvalid(2),
            axi_arready_o   => sig_slaves_arready(2),
            axi_rdata_o     => sig_slaves_rdata(2),
            axi_rresp_o     => sig_slaves_rresp(2),
            axi_rvalid_o    => sig_slaves_rvalid(2),
            axi_rready_i    => sig_slaves_rready(2),
            -- UARTLite Interface Signals
            rx_i    => uart_rx_i,
            tx_o    => uart_tx_o,
            baud_o  => uart_baud_o);


    -- -----------------------------------------------------
    -- Slave3 - GPIO
    -- -----------------------------------------------------
            
    gpio: gpio_axi4lite 
        port map(
            -- System signals
            clk_i       => clk_i,
            axi_rst_i   => reset_i,
            -- AXI signals
            axi_awaddr_i    => sig_slaves_awaddr(3),
            axi_awvalid_i   => sig_slaves_awvalid(3),
            axi_awready_o   => sig_slaves_awready(3),
            axi_wdata_i     => sig_slaves_wdata(3),
            axi_wstrb_i     => sig_slaves_wstrb(3),
            axi_wvalid_i    => sig_slaves_wvalid(3),
            axi_wready_o    => sig_slaves_wready(3),
            axi_bresp_o     => sig_slaves_bresp(3),
            axi_bvalid_o    => sig_slaves_bvalid(3),
            axi_bready_i    => sig_slaves_bready(3),
            axi_araddr_i    => sig_slaves_araddr(3),
            axi_arvalid_i   => sig_slaves_arvalid(3),
            axi_arready_o   => sig_slaves_arready(3),
            axi_rdata_o     => sig_slaves_rdata(3),
            axi_rresp_o     => sig_slaves_rresp(3),
            axi_rvalid_o    => sig_slaves_rvalid(3),
            axi_rready_i    => sig_slaves_rready(3),
            -- GPIO
            gpio_o  => gpio_o,
            gpio_i  => gpio_i);

    -- -----------------------------------------------------
    -- Slave4 - Timer
    -- -----------------------------------------------------
            
    timer: timer_axi4lite 
        port map(
            -- System signals
            clk_i       => clk_i,
            axi_rst_i   => reset_i,
            -- AXI signals
            axi_awaddr_i    => sig_slaves_awaddr(4),
            axi_awvalid_i   => sig_slaves_awvalid(4),
            axi_awready_o   => sig_slaves_awready(4),
            axi_wdata_i     => sig_slaves_wdata(4),
            axi_wstrb_i     => sig_slaves_wstrb(4),
            axi_wvalid_i    => sig_slaves_wvalid(4),
            axi_wready_o    => sig_slaves_wready(4),
            axi_bresp_o     => sig_slaves_bresp(4),
            axi_bvalid_o    => sig_slaves_bvalid(4),
            axi_bready_i    => sig_slaves_bready(4),
            axi_araddr_i    => sig_slaves_araddr(4),
            axi_arvalid_i   => sig_slaves_arvalid(4),
            axi_arready_o   => sig_slaves_arready(4),
            axi_rdata_o     => sig_slaves_rdata(4),
            axi_rresp_o     => sig_slaves_rresp(4),
            axi_rvalid_o    => sig_slaves_rvalid(4),
            axi_rready_i    => sig_slaves_rready(4),
            -- Int
            int_o  => sig_int_timer); 
            
    -- -----------------------------------------------------
    -- Slave5 - PIC
    -- -----------------------------------------------------
        
    sig_irqs(0) <= sig_int_timer;
    sig_irqs(1) <= sig_uart_rx_int;
    sig_irqs(2) <= sig_uart_tx_int;
    sig_irqs(10 downto 3) <= ext_int_i;
    sig_irqs(11) <= sig_noc_int;
    sig_irqs(31 downto 12) <= (others => '0');        
            
    pic: pic_axi4lite 
        port map(
            -- System signals
            clk_i       => clk_i,
            axi_rst_i   => reset_i,
            -- AXI signals
            axi_awaddr_i    => sig_slaves_awaddr(5),
            axi_awvalid_i   => sig_slaves_awvalid(5),
            axi_awready_o   => sig_slaves_awready(5),
            axi_wdata_i     => sig_slaves_wdata(5),
            axi_wstrb_i     => sig_slaves_wstrb(5),
            axi_wvalid_i    => sig_slaves_wvalid(5),
            axi_wready_o    => sig_slaves_wready(5),
            axi_bresp_o     => sig_slaves_bresp(5),
            axi_bvalid_o    => sig_slaves_bvalid(5),
            axi_bready_i    => sig_slaves_bready(5),
            axi_araddr_i    => sig_slaves_araddr(5),
            axi_arvalid_i   => sig_slaves_arvalid(5),
            axi_arready_o   => sig_slaves_arready(5),
            axi_rdata_o     => sig_slaves_rdata(5),
            axi_rresp_o     => sig_slaves_rresp(5),
            axi_rvalid_o    => sig_slaves_rvalid(5),
            axi_rready_i    => sig_slaves_rready(5),
            -- Int
            irq_i  => sig_irqs,
            int_o  => sig_intr);
            
    
    -- -----------------------------------------------------
    -- Slave6 - SoC TDM
    -- -----------------------------------------------------
    rtsnoc_router_axi: rtsnoc_axi4lite_slave
        generic map (
            NOC_LOCAL_ADR   => NODE_AXI_ADDR, 
            NOC_X           => ROUTER_X,
            NOC_Y           => ROUTER_Y,
            SOC_SIZE_X      => NET_SIZE_X_LOG2,
            SOC_SIZE_Y      => NET_SIZE_Y_LOG2,
            NOC_DATA_WIDTH  => NET_DATA_WIDTH)
        port map(
            -- System signals
            clk_i       => clk_i,
            axi_rst_i   => reset_i,
            -- AXI signals
            axi_awaddr_i    => sig_slaves_awaddr(6),
            axi_awvalid_i   => sig_slaves_awvalid(6),
            axi_awready_o   => sig_slaves_awready(6),
            axi_wdata_i     => sig_slaves_wdata(6),
            axi_wstrb_i     => sig_slaves_wstrb(6),
            axi_wvalid_i    => sig_slaves_wvalid(6),
            axi_wready_o    => sig_slaves_wready(6),
            axi_bresp_o     => sig_slaves_bresp(6),
            axi_bvalid_o    => sig_slaves_bvalid(6),
            axi_bready_i    => sig_slaves_bready(6),
            axi_araddr_i    => sig_slaves_araddr(6),
            axi_arvalid_i   => sig_slaves_arvalid(6),
            axi_arready_o   => sig_slaves_arready(6),
            axi_rdata_o     => sig_slaves_rdata(6),
            axi_rresp_o     => sig_slaves_rresp(6),
            axi_rvalid_o    => sig_slaves_rvalid(6),
            axi_rready_i    => sig_slaves_rready(6),
            -- NoC signals
            noc_reset_o => sig_noc_reset,
            noc_din_o   => sig_noc_nn_din,
            noc_dout_i  => sig_noc_nn_dout,
            noc_wr_o    => sig_noc_nn_wr,
            noc_rd_o    => sig_noc_nn_rd,
            noc_wait_i  => sig_noc_nn_wait,
            noc_nd_i    => sig_noc_nn_nd,
            -- NoC int
            noc_int_o   => sig_noc_int
        );  
    
    
    rtsnoc_router: ROUTER 
        GENERIC MAP (
            p_X         => conv_integer(ROUTER_X),
            p_Y         => conv_integer(ROUTER_Y),
            p_DATA      => NET_DATA_WIDTH,                    
            p_SIZE_X    => NET_SIZE_X,
            p_SIZE_Y    => NET_SIZE_Y)
        PORT MAP(
            i_CLK       => clk_i,
            i_RST       => sig_noc_reset,
            -- NORTH
            i_DIN_NN    => sig_noc_nn_din,
            o_DOUT_NN   => sig_noc_nn_dout,
            i_WR_NN     => sig_noc_nn_wr,
            i_RD_NN     => sig_noc_nn_rd,
            o_WAIT_NN   => sig_noc_nn_wait,
            o_ND_NN     => sig_noc_nn_nd,
            -- NORTHEAST
            i_DIN_NE    => sig_noc_ne_din,
            o_DOUT_NE   => sig_noc_ne_dout,
            i_WR_NE     => sig_noc_ne_wr,
            i_RD_NE     => sig_noc_ne_rd,
            o_WAIT_NE   => sig_noc_ne_wait,
            o_ND_NE     => sig_noc_ne_nd,
            -- EAST
            i_DIN_EE    => sig_noc_ee_din,
            o_DOUT_EE   => sig_noc_ee_dout,
            i_WR_EE     => sig_noc_ee_wr,
            i_RD_EE     => sig_noc_ee_rd,
            o_WAIT_EE   => sig_noc_ee_wait,
            o_ND_EE     => sig_noc_ee_nd,
            -- SOUTHEAST
            i_DIN_SE    => (others => '0'),
            o_DOUT_SE   => open,
            i_WR_SE     => '0',
            i_RD_SE     => '0',
            o_WAIT_SE   => open,
            o_ND_SE     => open,
            -- SOUTH
            i_DIN_SS    => (others => '0'),
            o_DOUT_SS   => open,
            i_WR_SS     => '0',
            i_RD_SS     => '0',
            o_WAIT_SS   => open,
            o_ND_SS     => open,
            -- SOUTHWEST
            i_DIN_SW    => (others => '0'),
            o_DOUT_SW   => open,
            i_WR_SW     => '0',
            i_RD_SW     => '0',
            o_WAIT_SW   => open,
            o_ND_SW     => open,
            -- WEST
            i_DIN_WW    => (others => '0'),
            o_DOUT_WW   => open,
            i_WR_WW     => '0',
            i_RD_WW     => '0',
            o_WAIT_WW   => open,
            o_ND_WW     => open,
            -- NORTHWEST
            i_DIN_NW    => (others => '0'),
            o_DOUT_NW   => open,
            i_WR_NW     => '0',
            i_RD_NW     => '0',
            o_WAIT_NW   => open,
            o_ND_NW     => open 
    );
    
    echo_node : rtsnoc_echo
        generic map (
            P0_ADDR          => NODE_ECHO_P0, 
            P0_ADDR_X        => ROUTER_X,
            P0_ADDR_Y        => ROUTER_Y,
            P1_ADDR          => NODE_ECHO_P1, 
            P1_ADDR_X        => ROUTER_X,
            P1_ADDR_Y        => ROUTER_Y,
            SOC_SIZE_X      => NET_SIZE_X_LOG2,
            SOC_SIZE_Y      => NET_SIZE_Y_LOG2,
            NOC_DATA_WIDTH  => NET_DATA_WIDTH)
        port map(
            -- System signals
            clk_i       => clk_i,
            rst_i       => sig_noc_reset,
            -- NoC signals
            p0_din_o   => sig_noc_ne_din,
            p0_dout_i  => sig_noc_ne_dout,
            p0_wr_o    => sig_noc_ne_wr,
            p0_rd_o    => sig_noc_ne_rd,
            p0_wait_i  => sig_noc_ne_wait,
            p0_nd_i    => sig_noc_ne_nd,
            p1_din_o   => sig_noc_ee_din,
            p1_dout_i  => sig_noc_ee_dout,
            p1_wr_o    => sig_noc_ee_wr,
            p1_rd_o    => sig_noc_ee_rd,
            p1_wait_i  => sig_noc_ee_wait,
            p1_nd_i    => sig_noc_ee_nd
        );

            

end RTL;
