library ieee;
use ieee.std_logic_1164.all;

entity plasma_axi4lite_ml605 is
    port(
        reset_btn    : in std_logic;
        
        clk_fpga_p   : in std_logic;
        clk_fpga_n   : in std_logic;

        uart_rx      : in std_logic;
        uart_tx      : out std_logic);
end plasma_axi4lite_ml605;

architecture RTL of plasma_axi4lite_ml605 is
    component clk_xlnx_100M_diff is
        port(
            CLK_IN1_P : in std_logic;
            CLK_IN1_N : in std_logic;
            CLK_OUT1  : out std_logic;
            CLK_OUT2  : out std_logic);
    end component;

    component plasma_axi4lite is
    generic(
        CLK_FREQ : integer);

    port(clk_i          : in std_logic;
        reset_i        : in std_logic;

        uart_tx_o   : out std_logic;
        uart_rx_i    : in std_logic);
    end component;


    signal clk_100MHz  : std_logic;
    signal clk_50MHz   : std_logic;
    signal sig_reset   : std_logic;
   
begin
        
    -- AXI reset is active-LOW
    sig_reset <= not reset_btn;

    clock_manager: clk_xlnx_100M_diff
        port map(
            CLK_IN1_P => clk_fpga_p,
            CLK_IN1_N => clk_fpga_n,
            CLK_OUT1  => clk_100MHz,
            CLK_OUT2  => clk_50MHz);

    plasma : plasma_axi4lite
    generic map(
        CLK_FREQ  => 50_000_000
    )
    port map(
        clk_i     => clk_50MHz,
        reset_i   => sig_reset,
        uart_tx_o => uart_tx,
        uart_rx_i => uart_rx
    );
    
    

end RTL;
