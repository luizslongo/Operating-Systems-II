library ieee;
use ieee.std_logic_1164.all


entity plasma_axi4lite_master is
    Port(
        aclk       : in std_logic,
        areset     : in std_logic,

        -- write address channel
        awvalid    : out std_logic,
        awready    : in std_logic,
        awaddr     : out std_logic_vector(31 downto 0),
        awprot     : out std_logic_vector(2 downto 0),

        -- write data channel
        wvalid     : out std_logic,
        wready     : in std_logic,
        wdata      : out std_logic_vector(31 downto 0),
        wstrb      : in std_logic_vector(3 downto 0),

        -- write response channel
        bvalid     : in std_logic,
        bready     : out std_logic,
        bresp      : in std_logic_vector(1 downto 0),

        -- read address channel
        arvalid    : out std_logic,
        arready    : in std_logic,
        araddr     : out std_logic_vector(31 downto 0),
        arprot     : in std_logic_vector(2 downto 0),

        -- read data channel
        rvalid     : in std_logic,
        rready     : out std_logic,
        rdata      : in std_logic_vector(31 downto 0),
        rresp      : out std_logic_vector(1 downto 0))
end plasma_axi4lite_master;


architecture RTL of plasma_axi4lite_master is
    component plasma is
        generic(
            memory_type : string := "XILINX_16X"; --"DUAL_PORT_" "ALTERA_LPM";
            log_file    : string := "UNUSED";
            ethernet    : integer := 0;
            use_cache   : integer := 0);
        Port(
            clk          : in std_logic;
            reset        : in std_logic;
            uart_write   : out std_logic;
            uart_read    : in std_logic;
            address      : out std_logic_vector(31 downto 2);
            byte_we      : out std_logic_vector(3 downto 0);
            data_write   : out std_logic_vector(31 downto 0);
            data_read    : in std_logic_vector(31 downto 0);
            mem_pause_in : in std_logic;
            no_ddr_start : out std_logic;
            no_ddr_stop  : out std_Logic;
            gpio0_out    : out std_logic_vector(31 downto 0);
            gpioA_in     : in std_logic_vector(31 downto 0));
    end component;


begin

    plasma_mcu: plasma
        generic map(
            memory_type => "XILINX_16X",
            log_file    => "UNUSED",
            ethernet    => 0,
            use_cache   => 0)
        port map(
            clk          => aclk,
            reset        => areset,

            address      => -- awaddr, araddr
            data_write   => -- wdata
            data_read    => -- rdata
            byte_we      => 
            mem_pause_in => 
            no_ddr_start => 
            no_ddr_stop  => 

            uart_write   => 
            uart_read    => 
            gpio0_out    => 
            gpioA_in     => )
end RTL;

