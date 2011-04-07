library ieee;
use ieee.std_logic_1164.all;

entity plasma_ml605 is
    Port(
        gpio_leds    : out std_logic_vector(7 downto 0);
        gpio_sws     : in std_logic_vector(7 downto 0);
        dir_leds     : out std_logic_vector(4 downto 0);
        dir_btns     : in std_logic_vector(4 downto 0);

        uart_tx_o    : out std_logic;
        uart_rx_i    : in std_logic;

        clk_fpga_p   : in std_logic;
        clk_fpga_n   : in std_logic);
end plasma_ml605;

architecture Behavioral of plasma_ml605 is
    component clk_xlnx_100M_diff is
        Port(
            CLK_IN1_P : in std_logic;
            CLK_IN1_N : in std_logic;
            CLK_OUT1  : out std_logic;
            CLK_OUT2  : out std_logic);
    end component;

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

    component fake_external_ram is
        Port(
            clk_i         : in std_logic;
            address       : in std_logic_vector(31 downto 2);
            byte_we       : in std_logic_vector(3 downto 0);
            data_write    : in std_logic_vector(31 downto 0);
            data_read     : out std_logic_vector(31 downto 0);
            mem_pause_out : out std_logic;
            no_ddr_start  : in std_logic;
            no_ddr_stop   : in std_logic);
    end component;

    signal clk_100MHz      : std_logic;
    signal clk_50MHz       : std_logic;
    signal reset           : std_logic;
    signal gpio_others     : std_logic_vector(18 downto 0);
    signal gpio0_out_s     : std_logic_vector(31 downto 0);
    signal gpioA_in_s      : std_logic_vector(31 downto 0);    

    signal extram_address    : std_logic_vector(31 downto 2);
    signal extram_byte_we    : std_logic_vector(3 downto 0);
    signal extram_data_write : std_logic_vector(31 downto 0);
    signal extram_data_read  : std_logic_vector(31 downto 0);

    signal extram_mem_pause    : std_logic;
    signal extram_no_ddr_start : std_logic;
    signal extram_no_ddr_stop  : std_logic;
begin

        clock_manager: clk_xlnx_100M_diff
            port map(
                CLK_IN1_P => clk_fpga_p,
                CLK_IN1_N => clk_fpga_n,
                CLK_OUT1  => clk_100MHz,
                CLK_OUT2  => clk_50MHz);

        
        reset       <= dir_btns(4);
        gpio_others <= gpio0_out_s(31 downto 13);
        dir_leds    <= gpio0_out_s(12 downto 8);
        gpio_leds   <= gpio0_out_s(7 downto 0);
        gpioA_in_s  <= "0000000000000000000" & dir_btns & gpio_sws;

        plasma_mcu: plasma
            generic map(
                memory_type => "XILINX_16X",
                log_file    => "UNUSED",
                ethernet    => 0,
                use_cache   => 0)
            port map(
                clk          => clk_50MHz,
                reset        => reset,
                uart_write   => uart_tx_o,
                uart_read    => uart_rx_i,
                address      => extram_address,
                byte_we      => extram_byte_we,
                data_write   => extram_data_write,
                data_read    => extram_data_read,
                mem_pause_in => extram_mem_pause,
                no_ddr_start => extram_no_ddr_start,
                no_ddr_stop  => extram_no_ddr_stop,
                gpio0_out    => gpio0_out_s,
                gpioA_in     => gpioA_in_s);


        fake_dram: fake_external_ram
            port map(
                clk_i         => clk_100MHz,
                address       => extram_address,
                byte_we       => extram_byte_we,
                data_write    => extram_data_write,
                data_read     => extram_data_read,
                mem_pause_out => extram_mem_pause,
                no_ddr_start  => extram_no_ddr_start,
                no_ddr_stop   => extram_no_ddr_stop);

end Behavioral;
