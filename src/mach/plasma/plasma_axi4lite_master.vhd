library ieee;
use ieee.std_logic_1164.all


entity plasma_axi4lite_master is
    Port(
        aclk       : in std_logic,
        areset     : in std_logic,

        -- write address channel
--        awvalid    : out std_logic,
--        awready    : in std_logic,
        awaddr     : out std_logic_vector(31 downto 0),
        awprot     : out std_logic_vector(2 downto 0),

        -- write data channel
--        wvalid     : out std_logic,
--        wready     : in std_logic,
        wdata      : out std_logic_vector(31 downto 0),
        wstrb      : in std_logic_vector(3 downto 0),

        -- write response channel
        bvalid     : in std_logic,
        bready     : out std_logic,
        bresp      : in std_logic_vector(1 downto 0),

        -- read address channel
--        arvalid    : out std_logic,
--        arready    : in std_logic,
        araddr     : out std_logic_vector(31 downto 0),
        arprot     : in std_logic_vector(2 downto 0),

        -- read data channel
--        rvalid     : in std_logic,
--        rready     : out std_logic,
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

    signal awprot, arprot   : std_logic_vector(2 downto 0);
    signal bready           : std_logic;

    signal plasma_uart_write    : std_logic;
    signal plasma_uart_read     : std_logic;
    signal plasma_address       : std_logic_vector(31 downto 2);
    signal plasma_byte_we       : std_logic_vector(3 downto 0);
    signal plasma_data_write    : std_logic_vector(31 downto 0);
    signal plasma_data_read     : std_logic_vector(31 downto 0);
    signal plasma_mem_pause_in  : std_logic;
    signal plasma_no_ddr_start  : std_logic;
    signal plasma_no_ddr_stop   : std_logic;
    signal plasma_gpio0_out     : std_logic_vector(31 downto 0);
    signal plasma_gpioA_in      : std_logic_vector(31 downto 0);

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

            address      => plasma_address,
            data_write   => plasma_data_write,
            data_read    => plasma_data_read,
            byte_we      => plasma_byte_we,
            mem_pause_in => plasma_mem_pause_in,
            no_ddr_start => plasma_no_ddr_start,
            no_ddr_stop  => plasma_no_ddr_stop,

            uart_write   => plasma_uart_write,
            uart_read    => plasma_uart_read,
            gpio0_out    => plasma_gpio0_out,
            gpioA_in     => plasma_gpioA_in)

    -- binding plasma MCU useless input pins to default signals
    plasma_uart_read <= '0';
    mem_pause_in     <= '0';
    gpioA_in         <= "00000000000000000000000000000000";


    -- state machine
    type STATE_TYPE is (READ_BEGIN, AR_READY, R_VALID,
                        WRITE_BEGIN, AW_READY, W_READY, WR_VALID, WR_RESP);

    signal current_state, next_state : STATE_TYPE;


    state_change: process(aclk, areset)
    begin
        if areset = '1' then
            current_state <= READ_BEGIN;
        elsif rising_edge(aclk) then
            current_state <= next_state;
        end if;
    end process;


    -- Moore machine, the inputs are Plasma's external memory signals
    -- and the outputs are all AXI4Lite channels
    state_decision: process(aclk, plasma_byte_we)
    begin
        case current_state is
            when READ_BEGIN =>
                if plasma_byte_we /= "0000" then
                    next_state <= WRITE_BEGIN;
                else
                    -- follow the read chain of states: READ_BEGIN, AR_READY, R_VALID
                end if;

                -- axi4lite channel signal assignments here

            when WRITE_BEGIN =>
                if plasma_byte_we = "0000" then
                    next_state <= READ_BEGIN;
                else
                    -- write chain of states: WRITE_BEGIN, AW_READY, W_READY, WR_VALID, WR_RESP
                end if;

                -- axi4lite channel signal assignments here

        end case;
    end process;

end RTL;

