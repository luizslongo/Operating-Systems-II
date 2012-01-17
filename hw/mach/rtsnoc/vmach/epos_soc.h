/*
 * vistual_platform.h
 *
 *  Created on: Jan 13, 2012
 *      Author: tiago
 */

#ifndef VIRTUAL_PLATFORM_H_
#define VIRTUAL_PLATFORM_H_

#include <systemc.h>
#include <mach/common/rtsnoc_router/rtsnoc_router.h>
#include <mach/common/rtsnoc_router/rtsnoc_echo.h>
#include "proc_io_node.h"

SC_MODULE(epos_soc) {


	//Ports
	sc_in<bool> clk;
	sc_in<bool> rst;

	sc_in<sc_uint<32> > gpio_in;
	sc_out<sc_uint<32> > gpio_out;
	sc_in<bool>	ext_int[8];


	//Constants
	enum{
		NET_SIZE_X		= 1,
		NET_SIZE_Y		= 1,
		NET_SIZE_X_LOG2 = 1, // it should be "integer(ceil(log2(real(NET_SIZE_X))))" when NET_SIZE_X >= 2
		NET_SIZE_Y_LOG2	= 1, // it should be "integer(ceil(log2(real(NET_SIZE_Y))))" when NET_SIZE_Y >= 2
		ROUTER_X		= 0,
		ROUTER_Y		= 0,
		NET_DATA_WIDTH	= 32,
		ROUTER_N_PORTS	= 8,
		NET_BUS_SIZE 	= NET_DATA_WIDTH+(2*NET_SIZE_X_LOG2)+(2*NET_SIZE_Y_LOG2)+6
	};

	typedef rtsnoc_router<ROUTER_X,ROUTER_Y,NET_DATA_WIDTH,NET_SIZE_X_LOG2,NET_SIZE_Y_LOG2> rtsnoc_router_t;

	enum{
		//NoC node addressess
		NODE_PROC_IO_ADDR	= rtsnoc_router_t::ROUTER_NN,
		NODE_ECHO_P0		= rtsnoc_router_t::ROUTER_WW,
		NODE_ECHO_P1		= rtsnoc_router_t::ROUTER_SS
	};

	typedef proc_io_node<ROUTER_X,ROUTER_Y,
				NODE_PROC_IO_ADDR,
				NET_DATA_WIDTH,NET_SIZE_X_LOG2,NET_SIZE_Y_LOG2> proc_io_node_t;

	typedef rtsnoc_echo<ROUTER_X,ROUTER_Y,
				NODE_ECHO_P0,
				NODE_ECHO_P1,
				NET_DATA_WIDTH,NET_SIZE_X_LOG2,NET_SIZE_Y_LOG2> rtsnoc_echo_t;


	//Modules

	rtsnoc_router_t		router;

	proc_io_node_t		node_proc_io;
	rtsnoc_echo_t		node_echo;


	sc_signal<sc_uint<NET_BUS_SIZE> > noc_din[ROUTER_N_PORTS];
	sc_signal<sc_uint<NET_BUS_SIZE> > noc_dout[ROUTER_N_PORTS];
	sc_signal<bool> noc_wr[ROUTER_N_PORTS];
	sc_signal<bool> noc_rd[ROUTER_N_PORTS];
	sc_signal<bool> noc_wait[ROUTER_N_PORTS];
	sc_signal<bool> noc_nd[ROUTER_N_PORTS];

	SC_CTOR(epos_soc)
		:router("rtsnoc_router"),
		 node_proc_io("noc_proc_io"),
		 node_echo("noc_echo")
	{

		router.clk(clk);
		router.rst(rst);
		for (int i = 0; i < ROUTER_N_PORTS; ++i) {
			router.din[i](noc_din[i]);
			router.dout[i](noc_dout[i]);
			router.wr[i](noc_wr[i]);
			router.rd[i](noc_rd[i]);
			router.wait[i](noc_wait[i]);
			router.nd[i](noc_nd[i]);
		}

		node_proc_io.clk(clk);
		node_proc_io.rst(rst);
		node_proc_io.gpio_in(gpio_in);
		node_proc_io.gpio_out(gpio_out);
		for (int i = 0; i < 8; ++i) node_proc_io.ext_int[i](ext_int[i]);
		node_proc_io.noc_din(noc_din[NODE_PROC_IO_ADDR]);
		node_proc_io.noc_dout(noc_dout[NODE_PROC_IO_ADDR]);
		node_proc_io.noc_wr(noc_wr[NODE_PROC_IO_ADDR]);
		node_proc_io.noc_rd(noc_rd[NODE_PROC_IO_ADDR]);
		node_proc_io.noc_wait(noc_wait[NODE_PROC_IO_ADDR]);
		node_proc_io.noc_nd(noc_nd[NODE_PROC_IO_ADDR]);

		node_echo.clk(clk);
		node_echo.rst(rst);
		node_echo.din[0](noc_din[NODE_ECHO_P0]);
		node_echo.dout[0](noc_dout[NODE_ECHO_P0]);
		node_echo.wr[0](noc_wr[NODE_ECHO_P0]);
		node_echo.rd[0](noc_rd[NODE_ECHO_P0]);
		node_echo.wait[0](noc_wait[NODE_ECHO_P0]);
		node_echo.nd[0](noc_nd[NODE_ECHO_P0]);
		node_echo.din[1](noc_din[NODE_ECHO_P1]);
		node_echo.dout[1](noc_dout[NODE_ECHO_P1]);
		node_echo.wr[1](noc_wr[NODE_ECHO_P1]);
		node_echo.rd[1](noc_rd[NODE_ECHO_P1]);
		node_echo.wait[1](noc_wait[NODE_ECHO_P1]);
		node_echo.nd[1](noc_nd[NODE_ECHO_P1]);

	}

	bool load_bootloader(const char* file_name){ return node_proc_io.load_bootloader(file_name); }

	unsigned long int get_tsc(){ return node_proc_io.get_tsc();}

};




#endif /* VISTUAL_PLATFORM_H_ */
