library ieee;
use ieee.std_logic_1164.all; 


entity plasma_axi4lite_master is
    generic(
        memory_type     : string  := "XILINX_16X";
        mult_type       : string  := "DEFAULT"; --AREA_OPTIMIZED
        shifter_type    : string  := "DEFAULT"; --AREA_OPTIMIZED
        alu_type        : string  := "DEFAULT"; --AREA_OPTIMIZED
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
end plasma_axi4lite_master;


architecture RTL of plasma_axi4lite_master is
    component mlite_cpu is
        generic(
            memory_type     : string  := "XILINX_16X"; --ALTERA_LPM, or DUAL_PORT_
            mult_type       : string  := "DEFAULT"; --AREA_OPTIMIZED
            shifter_type    : string  := "DEFAULT"; --AREA_OPTIMIZED
            alu_type        : string  := "DEFAULT"; --AREA_OPTIMIZED
            pipeline_stages : natural := 2); --2 or 3
        port(
            clk          : in std_logic;
            reset_in     : in std_logic;
            intr_in      : in std_logic;

            address_next : out std_logic_vector(31 downto 2);
            byte_we_next : out std_logic_vector(3 downto 0); 

            address      : out std_logic_vector(31 downto 2);
            byte_we      : out std_logic_vector(3 downto 0);
            data_w       : out std_logic_vector(31 downto 0);
            data_r       : in std_logic_vector(31 downto 0);
            mem_pause    : in std_logic);
    end component;

    signal plasma_reset         : std_logic;

    signal plasma_address_next  : std_logic_vector(31 downto 2);
    signal plasma_byte_we_next  : std_logic_vector(3 downto 0);

    signal plasma_address       : std_logic_vector(31 downto 2);
    signal plasma_byte_we       : std_logic_vector(3 downto 0);
    signal plasma_data_write    : std_logic_vector(31 downto 0);
    signal plasma_data_read     : std_logic_vector(31 downto 0);
    signal plasma_mem_pause_in  : std_logic;

    type STATE_TYPE is (RST1, RST2, READ_BEGIN, AR_READY, R_VALID, 
                        WRITE_BEGIN, AW_READY, W_READY, WR_VALID);
    signal current_state, next_state : STATE_TYPE;

    constant ZERO_32BITS : std_logic_vector(31 downto 0) := "00000000000000000000000000000000";

begin

    plasma_cpu: mlite_cpu
        generic map(
            memory_type     => memory_type,
            mult_type       => mult_type,
            shifter_type    => shifter_type,
            alu_type        => alu_type,
            pipeline_stages => pipeline_stages)
        port map(
            clk          => aclk,
            reset_in     => plasma_reset,
            intr_in      => intr,

            address_next => plasma_address_next,
            byte_we_next => plasma_byte_we_next, 

            address      => plasma_address,
            byte_we      => plasma_byte_we,
            data_w       => plasma_data_write,
            data_r       => plasma_data_read,
            mem_pause    => plasma_mem_pause_in);


    -- leave write and read protections ALL LOW
    awprot <= "000";
    arprot <= "000";

    -- plasma reset is active HIGH while AXI is active LOW, damn!
    plasma_reset <= not areset;

    -- write strobe, tied to plasma's byte_we
    wstrb <= plasma_byte_we;


    -- Moore machine, the inputs are Plasma's external memory signals
    -- and the outputs are all AXI4Lite channels
    state_change: process(aclk, areset)
    begin
        if areset = '0' then
            current_state <= RST1;
        elsif rising_edge(aclk) then
            current_state <= next_state;
        end if;
    end process;

    state_process: process(current_state, plasma_byte_we, arready, rvalid,
                            awready, wready, bvalid)
    begin
        case current_state is
            when RST1 => next_state <= RST2;
            when RST2 => next_state <= READ_BEGIN;

            when READ_BEGIN =>
                if plasma_byte_we /= "0000" then
                    next_state <= WRITE_BEGIN;
                elsif arready = '1' then
                    next_state <= AR_READY;
                else
                    next_state <= READ_BEGIN;
                end if;
            
            when AR_READY =>
                if rvalid = '1' then
                    next_state <= R_VALID;
                else
                    next_state <= AR_READY;
                end if;
            
            when R_VALID =>
                next_state <= READ_BEGIN;

            when WRITE_BEGIN =>
                if plasma_byte_we = "0000" then
                    next_state <= READ_BEGIN;
                elsif awready = '1' then
                    next_state <= AW_READY;
                else
                    next_state <= WRITE_BEGIN;
                end if;

            when AW_READY =>
                if wready = '1' then
                    next_state <= W_READY;
                else
                    next_state <= AW_READY;
                end if;

            when W_READY =>
                if bvalid = '1' then
                    next_state <= WR_VALID;
                else
                    next_state <= W_READY;
                end if;

            when WR_VALID =>
                next_state <= READ_BEGIN;
        end case;
    end process;

    outputs_process: process(areset, current_state,
                            plasma_address, plasma_data_write, rdata)
    begin
        if areset = '0' then
            -- must drive these LOW on reset, see: AXI Protocol Specification sec. 11.1.2
            arvalid <= '0';
            awvalid <= '0';
            wvalid  <= '0';
        else
            -- all outputs LOW by default
            awvalid <= '0'; awaddr <= ZERO_32BITS;
            wvalid  <= '0'; wdata  <= ZERO_32BITS;
            bready  <= '0';
            arvalid <= '0'; araddr <= ZERO_32BITS;
            rready  <= '0';
            plasma_data_read <= ZERO_32BITS;
            plasma_mem_pause_in <= '0';

            case current_state is
                when RST1 => arvalid <= '0'; awvalid <= '0'; wvalid  <= '0';
                when RST2 => arvalid <= '0'; awvalid <= '0'; wvalid  <= '0';

                when READ_BEGIN =>
                    plasma_mem_pause_in <= '0';
                    arvalid <= '1';
                    araddr  <= plasma_address & "00";

                when AR_READY =>
                    arvalid             <= '0';
                    plasma_mem_pause_in <= '1';
                    rready              <= '1';

                when R_VALID =>
                    plasma_mem_pause_in <= '1';
                    plasma_data_read    <= rdata;
                    rready              <= '0';

                when WRITE_BEGIN =>
                    awvalid <= '1';
                    awaddr  <= plasma_address & "00";

                when AW_READY =>
                    awvalid             <= '0';
                    plasma_mem_pause_in <= '1';
                    wvalid              <= '1';

                when W_READY =>
                    wvalid              <= '0';
                    plasma_mem_pause_in <= '1';
                    wdata               <= plasma_data_write;
                    bready              <= '1';

                when WR_VALID =>
                    plasma_mem_pause_in <= '1';
            end case;
        end if;
    end process;

end RTL;

