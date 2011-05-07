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
        uart_rx_i    : in std_logic);
    end component;


    signal uart_rx  : std_logic;
    signal uart_tx  : std_logic;

    signal clk_50MHz   : std_logic;
    signal sig_reset   : std_logic;
 
begin

    
    plasma: plasma_axi4lite
    generic map(
        CLK_FREQ  => 50_000_000
    )
    port map(
        clk_i     => clk_50MHz,
        reset_i   => sig_reset,
        uart_tx_o => uart_tx,
        uart_rx_i => uart_rx
    );
    
    
    -- simulation
    clk_process: process
    begin
        clk_50MHz <= '1';
        wait for 10 ns;
        clk_50MHz <= '0';
        wait for 10 ns;
    end process;

    tb : process
    begin
        sig_reset <= '0';
        wait for 60 ns;
        sig_reset <= '1';

        wait for 500 us;

        finish(0);
    end process;

end Behavioral;
