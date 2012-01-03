library ieee;
use ieee.std_logic_1164.all;

entity ml605_rtsnoc_axi4lite is
    port(
        reset_btn    : in std_logic;
        
        clk_fpga_p   : in std_logic;
        clk_fpga_n   : in std_logic;
        
        clk_sma_p    : in std_logic;
        clk_sma_n    : in std_logic;

        uart_rx      : in std_logic;
        uart_tx      : out std_logic;
        gpio_leds    : out std_logic_vector(7 downto 0);
        gpio_sws     : in std_logic_vector(7 downto 0);
        dir_leds     : out std_logic_vector(3 downto 0);
        dir_btns     : in std_logic_vector(3 downto 0));
end ml605_rtsnoc_axi4lite;

architecture rtl of ml605_rtsnoc_axi4lite is
    component clk_xlnx_100M_diff is
        port(
            CLK_IN1_P : in std_logic;
            CLK_IN1_N : in std_logic;
            CLK_IN2_P : in std_logic;
            CLK_IN2_N : in std_logic;
            CLK_IN_SEL : in std_logic;
            CLK_OUT1  : out std_logic;
            CLK_OUT2  : out std_logic;
            RESET     : in  std_logic);
    end component;

    component axi4lite_old is
        generic(CLK_FREQ : integer);
        port(
            clk_i       : in std_logic;
            reset_i     : in std_logic;
            uart_tx_o   : out std_logic;
            uart_rx_i   : in std_logic;
            uart_baud_o : out std_logic;
            gpio_i      : in  std_logic_vector(31 downto 0);
            gpio_o      : out  std_logic_vector(31 downto 0);
            ext_int_i   : in std_logic_vector(7 downto 0));
    end component;
    
    component axi4_reset_control is
        port(
            clk_i       : in std_logic;
            ext_reset_i : in std_logic;
            axi_reset_o : out std_logic);
    end component;

    signal sig_GND     : std_logic;
    
    signal clk_100MHz  : std_logic;
    signal clk_50MHz   : std_logic;
    signal sig_reset   : std_logic;
    
    
    signal sig_gpio_i : std_logic_vector(31 downto 0);
    signal sig_gpio_o : std_logic_vector(31 downto 0);
    
    signal sig_ext_int : std_logic_vector(7 downto 0);
   
begin
    
    sig_GND <= '0';
           
    clock_manager: clk_xlnx_100M_diff
        port map(
            CLK_IN1_P   => clk_fpga_p,
            CLK_IN1_N   => clk_fpga_n,
            CLK_IN2_P   => clk_sma_p,
            CLK_IN2_N   => clk_sma_n,
            CLK_IN_SEL  => '1',
            CLK_OUT1    => clk_100MHz,
            CLK_OUT2    => clk_50MHz,
            RESET       => sig_GND);

    -- AXI reset is active-LOW
    -- Force a initial reset
    reset: axi4_reset_control
        port map(
            clk_i => clk_50MHz,
            ext_reset_i => not reset_btn,
            axi_reset_o => sig_reset);
    
    -- TODO
    -- For now using the older verison (axi4lite_old) with both processor and 
    -- peripherals in a single node. The detached version is not working (axi4lite).
    -- Debug and get it to work.
    soc : axi4lite_old
        generic map(CLK_FREQ => 50_000_000)
        port map(
            clk_i       => clk_50MHz,
            reset_i     => sig_reset,
            uart_tx_o   => uart_tx,
            uart_rx_i   => uart_rx,
            uart_baud_o => open,
            gpio_i	    => sig_gpio_i,
            gpio_o	    => sig_gpio_o,
            ext_int_i   => sig_ext_int);
    

    sig_gpio_i(11 downto 0)  <= dir_btns & gpio_sws;
    sig_gpio_i(31 downto 12) <= (others => sig_GND);

    gpio_leds <= sig_gpio_o(7 downto 0);
    dir_leds  <= sig_gpio_o(11 downto 8);
    
    sig_ext_int(3 downto 0) <= dir_btns;
    sig_ext_int(7 downto 4) <= (others => sig_GND);

end rtl;
