library ieee;
use ieee.std_logic_1164.all;
use ieee.math_real.log2;
use ieee.math_real.ceil;
library std;
use std.env.all;



entity plasma_axi4lite_testbench is
end plasma_axi4lite_testbench;

architecture Behavioral of plasma_axi4lite_testbench is
    
    component plasma_axi4lite is
    generic(
        CLK_FREQ : integer);

    port(clk_i          : in std_logic;
        reset_i        : in std_logic;

        uart_tx_o   : out std_logic;
        uart_rx_i    : in std_logic;
        uart_baud_o : out std_logic);
    end component;
    
    component uart_rx is
    port (
        baudclk    : in std_logic;
        rxd     : in std_logic);
    end component;


    signal sig_uart_rx  : std_logic;
    signal sig_uart_tx  : std_logic;
    signal sig_uart_baud  : std_logic;

    signal sig_clk_50MHz   : std_logic;
    signal sig_reset   : std_logic;
 
begin

    
    plasma: plasma_axi4lite
    generic map(
        CLK_FREQ  => 50_000_000
    )
    port map(
        clk_i     => sig_clk_50MHz,
        reset_i   => sig_reset,
        uart_tx_o => sig_uart_tx,
        uart_rx_i => sig_uart_rx,
        uart_baud_o => sig_uart_baud
    );
    
    uart: uart_rx
    port map (
        baudclk => sig_uart_baud,
        rxd  => sig_uart_tx
    );
        
    -- simulation
    clk_process: process
    begin
        sig_clk_50MHz <= '1';
        wait for 10 ns;
        sig_clk_50MHz <= '0';
        wait for 10 ns;
    end process;

    tb : process
    begin
        sig_reset <= '0';
        wait for 60 ns;
        sig_reset <= '1';

        wait for 5 ms;

        finish(0);
    end process;

end Behavioral;
