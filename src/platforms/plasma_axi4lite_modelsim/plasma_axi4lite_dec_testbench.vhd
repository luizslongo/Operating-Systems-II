library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.log2;
use ieee.math_real.ceil;
library std;
use std.env.all;



entity plasma_axi4lite_testbench is
end plasma_axi4lite_testbench;

architecture Behavioral of plasma_axi4lite_testbench is
    
    constant N_SLAVES       : integer := 2;
    constant N_SLAVES_LOG   : integer := integer(ceil(log2(real(N_SLAVES))));
    
    constant SLAVE0_ADDR_W  : integer := 15;
    constant SLAVE0_ADDR    : std_logic_vector(SLAVE0_ADDR_W-1 downto 0) 
             := b"0000" & b"0000" & b"0000" & b"000";
    constant SLAVE1_ADDR_W  : integer := 15;
    constant SLAVE1_ADDR    : std_logic_vector(SLAVE1_ADDR_W-1 downto 0) 
             := b"0000" & b"0000" & b"0000" & b"001";
    
    component axi4lite_decoder is
        generic(
            sw          : integer := 16;
            sw_log2     : integer := 4;
            s0_addr_w   : integer := SLAVE0_ADDR_W; 
            s0_addr     : std_logic_vector(SLAVE0_ADDR_W-1 downto 0) := (others => '0');
            s1_addr_w   : integer := SLAVE1_ADDR_W; 
            s1_addr     : std_logic_vector(SLAVE1_ADDR_W-1 downto 0) := (others => '0'));
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
            
             -- slave write address channel
            s_awvalid_o    : out std_logic_vector(N_SLAVES-1 downto 0);
            s_awready_i    : in std_logic_vector(N_SLAVES-1 downto 0);
            s_awaddr_o     : out std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_awprot_o     : out std_logic_vector((N_SLAVES*3)-1 downto 0);
            -- slave write data channel
            s_wvalid_o     : out std_logic_vector(N_SLAVES-1 downto 0);
            s_wready_i     : in std_logic_vector(N_SLAVES-1 downto 0);
            s_wdata_o      : out std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_wstrb_o      : out std_logic_vector((N_SLAVES*4)-1 downto 0);
            -- slave write response channel
            s_bvalid_i     : in std_logic_vector(N_SLAVES-1 downto 0);
            s_bready_o     : out std_logic_vector(N_SLAVES-1 downto 0);
            s_bresp_i      : in std_logic_vector((N_SLAVES*2)-1 downto 0);
            -- slave read address channel
            s_arvalid_o    : out std_logic_vector(N_SLAVES-1 downto 0);
            s_arready_i    : in std_logic_vector(N_SLAVES-1 downto 0);
            s_araddr_o     : out std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_arprot_o     : out std_logic_vector((N_SLAVES*3)-1 downto 0);
            -- slave read data channel
            s_rvalid_i     : in std_logic_vector(N_SLAVES-1 downto 0);
            s_rready_o     : out std_logic_vector(N_SLAVES-1 downto 0);
            s_rdata_i      : in std_logic_vector((N_SLAVES*32)-1 downto 0);
            s_rresp_i      : in std_logic_vector((N_SLAVES*2)-1 downto 0)
        );
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


    signal clk_50MHz   : std_logic;
    signal sig_reset   : std_logic;
    signal sig_intr    : std_logic;

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
    
    signal sig_s0_awvalid : std_logic;
    signal sig_s0_awready : std_logic;
    signal sig_s0_awaddr  : std_logic_vector(31 downto 0);
    signal sig_s0_awprot  : std_logic_vector(2 downto 0);

    signal sig_s0_wvalid  : std_logic;
    signal sig_s0_wready  : std_logic;
    signal sig_s0_wdata   : std_logic_vector(31 downto 0);
    signal sig_s0_wstrb   : std_logic_vector(3 downto 0);

    signal sig_s0_bvalid  : std_logic;
    signal sig_s0_bready  : std_logic;
    signal sig_s0_bresp   : std_logic_vector(1 downto 0);

    signal sig_s0_arvalid : std_logic;
    signal sig_s0_arready : std_logic;
    signal sig_s0_araddr  : std_logic_vector(31 downto 0);
    signal sig_s0_arprot  : std_logic_vector(2 downto 0);

    signal sig_s0_rvalid  : std_logic;
    signal sig_s0_rready  : std_logic;
    signal sig_s0_rdata   : std_logic_vector(31 downto 0);
    signal sig_s0_rresp   : std_logic_vector(1 downto 0);
    
    signal sig_s1_awvalid : std_logic;
    signal sig_s1_awready : std_logic;
    signal sig_s1_awaddr  : std_logic_vector(31 downto 0);
    signal sig_s1_awprot  : std_logic_vector(2 downto 0);

    signal sig_s1_wvalid  : std_logic;
    signal sig_s1_wready  : std_logic;
    signal sig_s1_wdata   : std_logic_vector(31 downto 0);
    signal sig_s1_wstrb   : std_logic_vector(3 downto 0);

    signal sig_s1_bvalid  : std_logic;
    signal sig_s1_bready  : std_logic;
    signal sig_s1_bresp   : std_logic_vector(1 downto 0);

    signal sig_s1_arvalid : std_logic;
    signal sig_s1_arready : std_logic;
    signal sig_s1_araddr  : std_logic_vector(31 downto 0);
    signal sig_s1_arprot  : std_logic_vector(2 downto 0);

    signal sig_s1_rvalid  : std_logic;
    signal sig_s1_rready  : std_logic;
    signal sig_s1_rdata   : std_logic_vector(31 downto 0);
    signal sig_s1_rresp   : std_logic_vector(1 downto 0);

begin

    -- ----------------------------------------------------------
    -- AXI4 Interconnect
    -- ----------------------------------------------------------
    axi4lite_interconnect: axi4lite_decoder
        generic map(
            sw          => N_SLAVES,
            sw_log2     => N_SLAVES_LOG,
            s0_addr_w   => SLAVE0_ADDR_W, 
            s0_addr     => SLAVE0_ADDR,
            s1_addr_w   => SLAVE1_ADDR_W, 
            s1_addr     => SLAVE1_ADDR
        )
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
            
             -- slave write address channel
            s_awvalid_o =>  sig_awvalid,
            s_awready_i =>  sig_awready,
            s_awaddr_o  =>  sig_awaddr,
            s_awprot_o  =>  sig_awprot,
            -- slave write data channel
            s_wvalid_o  =>  sig_wvalid,
            s_wready_i  =>  sig_wready,
            s_wdata_o   =>  sig_wdata,
            s_wstrb_o   =>  sig_wstrb,
            -- slave write response channel
            s_bvalid_i  =>  sig_bvalid,
            s_bready_o  =>  sig_bready,
            s_bresp_i   =>  sig_bresp,
            -- slave read address channel
            s_arvalid_o =>  sig_arvalid,
            s_arready_i =>  sig_arready,
            s_araddr_o  =>  sig_araddr,
            s_arprot_o  =>  sig_arprot,
            -- slave read data channel
            s_rvalid_i  =>  sig_rvalid,
            s_rready_o  =>  sig_rready,
            s_rdata_i   =>  sig_rdata,
            s_rresp_i   =>  sig_rresp
        );
        
     -- slave write address channel
     sig_s0_awvalid <= sig_awvalid(0);
     sig_s1_awvalid <= sig_awvalid(1);  
     sig_awready    <= sig_s1_awready & sig_s0_awready;
     sig_s0_awaddr  <= sig_awaddr(31 downto 0);
     sig_s1_awaddr  <= sig_awaddr(63 downto 32);
     sig_s0_awprot  <= sig_awprot(2 downto 0);
     sig_s1_awprot  <= sig_awprot(5 downto 3);
    -- slave write data channel
     sig_s0_wvalid  <= sig_wvalid(0);
     sig_s1_wvalid  <= sig_wvalid(1);
     sig_wready     <= sig_s1_wready & sig_s0_wready;
     sig_s0_wdata   <= sig_wdata(31 downto 0);
     sig_s1_wdata   <= sig_wdata(63 downto 32);
     sig_s0_wstrb   <= sig_wstrb(3 downto 0);
     sig_s1_wstrb   <= sig_wstrb(7 downto 4);
     -- slave write response channel
     sig_bvalid     <= sig_s1_bvalid & sig_s0_bvalid;
     sig_s0_bready  <= sig_bready(0);
     sig_s1_bready  <= sig_bready(1);
     sig_bresp      <= sig_s1_bresp & sig_s0_bresp;
    -- slave read address channel
     sig_s0_arvalid <= sig_arvalid(0);
     sig_s1_arvalid <= sig_arvalid(1);
     sig_arready    <= sig_s1_arready & sig_s0_arready;
     sig_s0_araddr  <= sig_araddr(31 downto 0);
     sig_s1_araddr  <= sig_araddr(63 downto 32);
     sig_s0_arprot  <= sig_arprot(2 downto 0);
     sig_s1_arprot  <= sig_arprot(5 downto 3);
    -- slave read data channel
     sig_rvalid     <= sig_s1_rvalid & sig_s0_rvalid;
     sig_s0_rready  <= sig_rready(0);
     sig_s1_rready  <= sig_rready(1);
     sig_rdata      <= sig_s1_rdata & sig_s0_rdata;
     sig_rresp      <= sig_s1_rresp & sig_s0_rresp;


    -- ----------------------------------------------------------
    -- Master
    -- ----------------------------------------------------------
    
    plasma_amba: plasma_axi4lite_master
        generic map(
            memory_type     => "XILINX_16X",
            mult_type       => "DEFAULT",
            shifter_type    => "DEFAULT",
            alu_type        => "DEFAULT",
            pipeline_stages => 2)
        port map(
            aclk       => clk_50MHz,
            areset     => sig_reset,
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

    -- ----------------------------------------------------------
    -- Slave 0
    -- ----------------------------------------------------------

    ram_amba: ram_amba_128k
        port map(
            s_aclk        => clk_50MHz,
            s_aresetn     => sig_reset,

            s_axi_awvalid => sig_s0_awvalid,
            s_axi_awready => sig_s0_awready,
            s_axi_awaddr  => sig_s0_awaddr, 

            s_axi_wvalid  => sig_s0_wvalid,
            s_axi_wready  => sig_s0_wready,
            s_axi_wdata   => sig_s0_wdata,
            s_axi_wstrb   => sig_s0_wstrb,

            s_axi_bvalid  => sig_s0_bvalid,
            s_axi_bready  => sig_s0_bready,
            s_axi_bresp   => sig_s0_bresp,

            s_axi_arvalid => sig_s0_arvalid,
            s_axi_arready => sig_s0_arready,
            s_axi_araddr  => sig_s0_araddr,

            s_axi_rvalid  => sig_s0_rvalid,
            s_axi_rready  => sig_s0_rready,
            s_axi_rdata   => sig_s0_rdata,
            s_axi_rresp   => sig_s0_rresp);
     
    -- ----------------------------------------------------------
    -- Slave 1
    -- ----------------------------------------------------------     
            
    device1: ram_amba_128k
        port map(
            s_aclk        => clk_50MHz,
            s_aresetn     => sig_reset,

            s_axi_awvalid => sig_s1_awvalid,
            s_axi_awready => sig_s1_awready,
            s_axi_awaddr  => sig_s1_awaddr, 

            s_axi_wvalid  => sig_s1_wvalid,
            s_axi_wready  => sig_s1_wready,
            s_axi_wdata   => sig_s1_wdata,
            s_axi_wstrb   => sig_s1_wstrb,

            s_axi_bvalid  => sig_s1_bvalid,
            s_axi_bready  => sig_s1_bready,
            s_axi_bresp   => sig_s1_bresp,

            s_axi_arvalid => sig_s1_arvalid,
            s_axi_arready => sig_s1_arready,
            s_axi_araddr  => sig_s1_araddr,

            s_axi_rvalid  => sig_s1_rvalid,
            s_axi_rready  => sig_s1_rready,
            s_axi_rdata   => sig_s1_rdata,
            s_axi_rresp   => sig_s1_rresp);

    -- ----------------------------------------------------------
    -- Simulation
    -- ----------------------------------------------------------

    clk_process: process
    begin
        clk_50MHz <= '1';
        wait for 10 ns;
        clk_50MHz <= '0';
        wait for 10 ns;
    end process;


    -- do not interrupt the CPU
    sig_intr <= '0';

    tb : process
    begin
        sig_reset <= '0';
        wait for 60 ns;
        sig_reset <= '1';

        wait for 100000 ns;

        finish(0);
    end process;

end Behavioral;
