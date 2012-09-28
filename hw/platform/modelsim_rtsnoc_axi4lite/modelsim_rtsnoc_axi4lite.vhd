library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.math_real.log2;
use ieee.math_real.ceil;
USE ieee.std_logic_unsigned.ALL;
library std;
use std.env.all;
--use STD.textio.all;



entity virtual_platform_hdl is
end virtual_platform_hdl;

architecture Behavioral of virtual_platform_hdl is
    
    component axi4lite_old is
    generic(
        CLK_FREQ : integer);

    port(clk_i          : in std_logic;
        reset_i        : in std_logic;

        uart_tx_o   : out std_logic;
        uart_rx_i    : in std_logic;
        uart_baud_o : out std_logic;
        gpio_i  : in  std_logic_vector(31 downto 0);
        gpio_o  : out  std_logic_vector(31 downto 0);
        ext_int_i  : in  std_logic_vector(7 downto 0));
    end component;
    
    component uart_rx is
    port (
        baudclk    : in std_logic;
        rxd     : in std_logic);
    end component;
    
    component gpio_model is
    port (
        gpio_i : in  std_logic_vector(31 downto 0));
    end component;


    signal sig_uart_rx  : std_logic;
    signal sig_uart_tx  : std_logic;
    signal sig_uart_baud  : std_logic;
    
    signal sig_gpio_i : std_logic_vector(31 downto 0);
    signal sig_gpio_o : std_logic_vector(31 downto 0);
    signal sig_ext_int : std_logic_vector(7 downto 0);

    signal sig_clk_50MHz   : std_logic;
    signal sig_reset   : std_logic;
    
    signal tsc : integer := 0;
 
begin

    
    soc: axi4lite_old
    generic map(
        CLK_FREQ  => 50_000_000
    )
    port map(
        clk_i     => sig_clk_50MHz,
        reset_i   => sig_reset,
        uart_tx_o => sig_uart_tx,
        uart_rx_i => sig_uart_rx,
        uart_baud_o => sig_uart_baud,
        gpio_i => sig_gpio_i,
        gpio_o => sig_gpio_o,
        ext_int_i => sig_ext_int
    );
    
    uart: uart_rx
    port map (
        baudclk => sig_uart_baud,
        rxd  => sig_uart_tx
    );
    
    gpioxx: gpio_model
    port map (
        gpio_i => sig_gpio_o
    );
        
    -- simulation
    clk_process: process
    begin
        sig_clk_50MHz <= '1';
        wait for 10 ns;
        sig_clk_50MHz <= '0';
        wait for 10 ns;
        tsc <= tsc + 1;
    end process;

    tb : process
    begin
        sig_reset <= '0';
        wait for 60 ns;
        sig_reset <= '1';

        wait for 1000 ms;

        finish(0);
    end process;
    
    sig_gpio_i <= "0000000000000000000000000000000";
    --sig_gpio_i <= "11111111111111111111111111111111";
    sig_ext_int <= "00000000";
    
    
end Behavioral;
