
-- VHDL Instantiation Created from source file ROUTER.vhd -- 16:29:52 02/20/2011
--
-- Notes: 
-- 1) This instantiation template has been automatically generated using types
-- std_logic and std_logic_vector for the ports of the instantiated module
-- 2) To use this template to instantiate this entity, cut-and-paste and then edit

	COMPONENT ROUTER
	GENERIC (
					p_X			: integer := 2;
					p_Y			: integer := 2;
					p_DATA		: integer := 16;					
					p_SIZE_X		: integer := 4;
					p_SIZE_Y		: integer := 4
	);	
	PORT(
			i_CLK 		: IN std_logic;
			i_RST 		: IN std_logic;
			i_DIN_NN 	: IN std_logic_vector(37 downto 0);
			i_WR_NN 		: IN std_logic;
			i_RD_NN 		: IN std_logic;
			i_DIN_NE 	: IN std_logic_vector(37 downto 0);
			i_WR_NE 		: IN std_logic;
			i_RD_NE 		: IN std_logic;
			i_DIN_EE 	: IN std_logic_vector(37 downto 0);
			i_WR_EE 		: IN std_logic;
			i_RD_EE 		: IN std_logic;
			i_DIN_SE 	: IN std_logic_vector(37 downto 0);
			i_WR_SE 		: IN std_logic;
			i_RD_SE 		: IN std_logic;
			i_DIN_SS 	: IN std_logic_vector(37 downto 0);
			i_WR_SS 		: IN std_logic;
			i_RD_SS 		: IN std_logic;
			i_DIN_SW 	: IN std_logic_vector(37 downto 0);
			i_WR_SW 		: IN std_logic;
			i_RD_SW 		: IN std_logic;
			i_DIN_WW 	: IN std_logic_vector(37 downto 0);
			i_WR_WW 		: IN std_logic;
			i_RD_WW 		: IN std_logic;
			i_DIN_NW 	: IN std_logic_vector(37 downto 0);
			i_WR_NW 		: IN std_logic;
			i_RD_NW 		: IN std_logic;          
			o_DOUT_NN 	: OUT std_logic_vector(37 downto 0);
			o_WAIT_NN 	: OUT std_logic;
			o_ND_NN 		: OUT std_logic;
			o_DOUT_NE 	: OUT std_logic_vector(37 downto 0);
			o_WAIT_NE 	: OUT std_logic;
			o_ND_NE 		: OUT std_logic;
			o_DOUT_EE 	: OUT std_logic_vector(37 downto 0);
			o_WAIT_EE 	: OUT std_logic;
			o_ND_EE 		: OUT std_logic;
			o_DOUT_SE 	: OUT std_logic_vector(37 downto 0);
			o_WAIT_SE 	: OUT std_logic;
			o_ND_SE 		: OUT std_logic;
			o_DOUT_SS 	: OUT std_logic_vector(37 downto 0);
			o_WAIT_SS 	: OUT std_logic;
			o_ND_SS 		: OUT std_logic;
			o_DOUT_SW 	: OUT std_logic_vector(37 downto 0);
			o_WAIT_SW 	: OUT std_logic;
			o_ND_SW 		: OUT std_logic;
			o_DOUT_WW 	: OUT std_logic_vector(37 downto 0);
			o_WAIT_WW 	: OUT std_logic;
			o_ND_WW 		: OUT std_logic;
			o_DOUT_NW 	: OUT std_logic_vector(37 downto 0);
			o_WAIT_NW 	: OUT std_logic;
			o_ND_NW 		: OUT std_logic
		);
	END COMPONENT;

	Inst_ROUTER: ROUTER 
	GENERIC MAP (
					p_X			=> 0,
					p_Y			=> 0,
					p_DATA		=> p_DATA,
					p_SIZE_X		=> p_SIZE_X,
					p_SIZE_Y		=> p_SIZE_Y
	)
	PORT MAP(
		i_CLK 		=> ,
		i_RST 		=> ,
		--
		-- NORTH
		--
		i_DIN_NN 	=> ,
		o_DOUT_NN 	=> ,
		i_WR_NN 		=> ,
		i_RD_NN 		=> ,
		o_WAIT_NN 	=> ,
		o_ND_NN	 	=> ,
		--
		-- NORTHEAST
		--
		i_DIN_NE 	=> ,
		o_DOUT_NE 	=> ,
		i_WR_NE 		=> ,
		i_RD_NE 		=> ,
		o_WAIT_NE 	=> ,
		o_ND_NE 		=> ,
		--
		-- EAST
		--
		i_DIN_EE 	=> ,
		o_DOUT_EE 	=> ,
		i_WR_EE 		=> ,
		i_RD_EE 		=> ,
		o_WAIT_EE 	=> ,
		o_ND_EE 		=> ,
		--
		-- SOUTHEAST
		--
		i_DIN_SE 	=> ,
		o_DOUT_SE 	=> ,
		i_WR_SE 		=> ,
		i_RD_SE 		=> ,
		o_WAIT_SE 	=> ,
		o_ND_SE 		=> ,
		--
		-- SOUTH
		--
		i_DIN_SS 	=> ,
		o_DOUT_SS 	=> ,
		i_WR_SS 		=> ,
		i_RD_SS 		=> ,
		o_WAIT_SS 	=> ,
		o_ND_SS 		=> ,
		--
		-- SOUTHWEST
		--
		i_DIN_SW 	=> ,
		o_DOUT_SW 	=> ,
		i_WR_SW 		=> ,
		i_RD_SW 		=> ,
		o_WAIT_SW 	=> ,
		o_ND_SW 		=> ,
		--
		-- WEST
		--
		i_DIN_WW 	=> ,
		o_DOUT_WW 	=> ,
		i_WR_WW 		=> ,
		i_RD_WW 		=> ,
		o_WAIT_WW 	=> ,
		o_ND_WW 		=> ,
		--
		-- NORTHWEST
		--
		i_DIN_NW 	=> ,
		o_DOUT_NW 	=> ,
		i_WR_NW 		=> ,
		i_RD_NW 		=> ,
		o_WAIT_NW 	=> ,
		o_ND_NW 		=> 
	);


