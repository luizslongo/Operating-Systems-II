/*
 * rtsnoc_echo.h
 *
 *  Created on: Jan 12, 2012
 *      Author: tiago
 */

#ifndef RTSNOC_ECHO_H_
#define RTSNOC_ECHO_H_

#include <systemc.h>

template<
	unsigned int X,
	unsigned int Y,
	unsigned int P0_LOCAL_ADDR,
	unsigned int P1_LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
SC_MODULE(rtsnoc_echo){

	enum{
		BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,

		MASK_SIZE_DATA = ~((-1) << SIZE_DATA),
		MASK_SIZE_X = ~((-1) << SIZE_X),
		MASK_SIZE_Y = ~((-1) << SIZE_Y),
		MASK_SIZE_LOCAL = 0x7,

		MASK_WR = 0x1,
		MASK_RD = 0x2,
	};

	sc_in<bool>		clk;
	sc_in<bool>		rst;

	sc_out<sc_uint<BUS_SIZE> > din[2];
	sc_in<sc_uint<BUS_SIZE> > dout[2];
	sc_out<bool> wr[2];
	sc_out<bool> rd[2];
	sc_in<bool> wait[2];
	sc_in<bool> nd[2];

	SC_CTOR(rtsnoc_echo){
		SC_CTHREAD(p0_process, clk.pos());
		reset_signal_is(rst, true);
		SC_CTHREAD(p1_process, clk.pos());
		reset_signal_is(rst, true);
	}

	void p0_process();
	void p1_process();

private:
	void echo_state_machine(unsigned int port, unsigned int port_inv, unsigned int rx_addr, unsigned int tx_addr);

};

template<
	unsigned int X,
	unsigned int Y,
	unsigned int P0_LOCAL_ADDR,
	unsigned int P1_LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
void rtsnoc_echo<X,Y,P0_LOCAL_ADDR,P1_LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y>::p0_process(){
	din[1] = 0;
	wr[1] = 0;
	rd[0] = 0;

	sc_module::wait();
	while(true){
		echo_state_machine(0,1,P0_LOCAL_ADDR,P1_LOCAL_ADDR);
		sc_module::wait();
	}
}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int P0_LOCAL_ADDR,
	unsigned int P1_LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
void rtsnoc_echo<X,Y,P0_LOCAL_ADDR,P1_LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y>::p1_process(){
	din[0] = 0;
	wr[0] = 0;
	rd[1] = 0;

	sc_module::wait();
	while(true){
		echo_state_machine(1,0,P1_LOCAL_ADDR,P0_LOCAL_ADDR);
		sc_module::wait();
	}
}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int P0_LOCAL_ADDR,
	unsigned int P1_LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
void rtsnoc_echo<X,Y,P0_LOCAL_ADDR,P1_LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y>::echo_state_machine(unsigned int port, unsigned int port_inv, unsigned int rx_addr, unsigned int tx_addr){
	sc_uint<SIZE_DATA>	tx_data;
	sc_uint<3> 			tx_local_dst;
	sc_uint<SIZE_Y>		tx_Y_dst;
	sc_uint<SIZE_X>		tx_X_dst;
	sc_uint<3>			tx_local_orig;
	sc_uint<SIZE_Y>		tx_Y_orig;
	sc_uint<SIZE_X>		tx_X_orig;

	sc_uint<SIZE_DATA>	rx_data;
	sc_uint<3> 			rx_local_dst;
	sc_uint<SIZE_Y>		rx_Y_dst;
	sc_uint<SIZE_X>		rx_X_dst;
	sc_uint<3>			rx_local_orig;
	sc_uint<SIZE_Y>		rx_Y_orig;
	sc_uint<SIZE_X>		rx_X_orig;

	(rx_X_orig,
	 rx_Y_orig,
	 rx_local_orig,
	 rx_X_dst,
	 rx_Y_dst,
	 rx_local_dst,
	 rx_data) = dout[port].read();


	if(nd[port].read()){

		tx_X_orig = rx_X_dst;
		tx_Y_orig = rx_Y_dst;
		tx_local_orig = tx_addr;//tx_local_orig <= rx_local_dst;
		tx_X_dst = rx_X_orig;
		tx_Y_dst = rx_Y_orig;
		tx_local_dst = rx_local_orig;
		tx_data = rx_data;

		rd[port] = true;

		sc_module::wait();

		rd[port] = false;
		while(wait[port_inv].read()) sc_module::wait();
		din[port_inv] = (tx_X_orig,
					 tx_Y_orig,
					 tx_local_orig,
					 tx_X_dst,
					 tx_Y_dst,
					 tx_local_dst,
					 tx_data);
		wr[port_inv] = true;

		sc_module::wait();

		wr[port_inv] = false;
	}
}



#endif /* RTSNOC_ECHO_H_ */
