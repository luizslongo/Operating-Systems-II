/*
 * rtsnoc_bus_proxy.h
 *
 *  Created on: Jan 12, 2012
 *      Author: tiago
 */

#ifndef RTSNOC_BUS_PROXY_H_
#define RTSNOC_BUS_PROXY_H_

#include <systemc.h>
#include <module_if.h>

template<
	unsigned int X,
	unsigned int Y,
	unsigned int LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
class rtsnoc_bus_proxy : public sc_module,
	public module_if {

public:

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
	sc_out<bool>	irq;

	sc_out<sc_uint<BUS_SIZE> > din;
	sc_in<sc_uint<BUS_SIZE> > dout;
	sc_out<bool> wr;
	sc_out<bool> rd;
	sc_in<bool> wait;
	sc_in<bool> nd;

	SC_HAS_PROCESS(rtsnoc_bus_proxy);
	rtsnoc_bus_proxy(sc_module_name nm_
			, unsigned int start_address_
			, unsigned int end_address_
			)
		: sc_module(nm_)
		, clk("clk")
		, rst("rst")
		, irq("irq")
		, start_address(start_address_)
		, end_address(end_address_)
	{
		SC_CTHREAD(int_process, clk.pos());
		reset_signal_is(rst, true);

		 noc_tx_data = 0;
		 noc_tx_local_dst = 0;
		 noc_tx_Y_dst = 0;
		 noc_tx_X_dst = 0;
		 noc_tx_local_orig = 0;
		 noc_tx_Y_orig = 0;
		 noc_tx_X_orig = 0;

	}

	~rtsnoc_bus_proxy() {

	}

	//interfaces
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

	unsigned int get_start_address() const;
	unsigned int get_end_address() const;

	void int_process();


private:

	unsigned int	start_address;
	unsigned int	end_address;

	sc_uint<SIZE_DATA>	noc_tx_data;
	sc_uint<3> 			noc_tx_local_dst;
	sc_uint<SIZE_Y>		noc_tx_Y_dst;
	sc_uint<SIZE_X>		noc_tx_X_dst;
	sc_uint<3>			noc_tx_local_orig;
	sc_uint<SIZE_Y>		noc_tx_Y_orig;
	sc_uint<SIZE_X>		noc_tx_X_orig;

	sc_signal<bool> noc_nd;




};


template<
	unsigned int X,
	unsigned int Y,
	unsigned int LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
void rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y>::read(unsigned int *data, unsigned int address, int size) {

	sc_uint<SIZE_DATA>	noc_rx_data;
	sc_uint<3> 			noc_rx_local_dst;
	sc_uint<SIZE_Y>		noc_rx_Y_dst;
	sc_uint<SIZE_X>		noc_rx_X_dst;
	sc_uint<3>			noc_rx_local_orig;
	sc_uint<SIZE_Y>		noc_rx_Y_orig;
	sc_uint<SIZE_X>		noc_rx_X_orig;

	(noc_rx_X_orig,
	 noc_rx_Y_orig,
	 noc_rx_local_orig,
	 noc_rx_X_dst,
	 noc_rx_Y_dst,
	 noc_rx_local_dst,
	 noc_rx_data) = dout.read();

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
		case 0: *data = noc_rx_local_dst; break;
		case 1: *data = noc_rx_Y_dst; break;
		case 2: *data = noc_rx_X_dst; break;
		case 3: *data = noc_rx_local_orig; break;
		case 4: *data = noc_rx_Y_orig; break;
		case 5: *data = noc_rx_X_orig; break;
		case 6: *data = noc_rx_data; break;
		case 7:{
			sc_uint<4> tmp = 0;
			tmp[0] = wr.read();
			tmp[1] = rd.read();
			tmp[2] = wait.read();
			tmp[3] = nd.read();
			*data = tmp.to_uint();
			break;
		}
		case 8: *data = LOCAL_ADDR; break;
		case 9: *data = X; break;
		case 10: *data = Y; break;
		case 11: *data = SIZE_X; break;
		case 12: *data = SIZE_Y; break;
		case 13: *data = SIZE_DATA; break;
		default:
			break;
	}



	sc_module::wait(clk.posedge_event());

}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
void rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y>::write(unsigned int data, unsigned int address, int size) {

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
		case 0: noc_tx_local_dst =	data & MASK_SIZE_LOCAL; break;
		case 1: noc_tx_Y_dst =		data & MASK_SIZE_Y;	break;
		case 2: noc_tx_X_dst =		data & MASK_SIZE_X;	break;
		case 3: noc_tx_local_orig =	data & MASK_SIZE_LOCAL; break;
		case 4: noc_tx_Y_orig =		data & MASK_SIZE_Y;	break;
		case 5: noc_tx_X_orig =		data & MASK_SIZE_X;	break;
		case 6: noc_tx_data =		data & MASK_SIZE_DATA; break;
		case 7:
			wr = data & MASK_WR;
			rd = data & MASK_RD;
			break;
		default:
			break;
	}

	din = (noc_tx_X_orig,
           noc_tx_Y_orig,
           noc_tx_local_orig,
           noc_tx_X_dst,
           noc_tx_Y_dst,
           noc_tx_local_dst,
           noc_tx_data);

	sc_module::wait(clk.posedge_event());
	wr = false;
	rd = false;
	sc_module::wait(clk.posedge_event());

}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
unsigned int rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y>::get_start_address() const{
	return start_address;
}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
unsigned int rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y>::get_end_address() const{
	return end_address;
}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int LOCAL_ADDR,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
void rtsnoc_bus_proxy<X,Y,LOCAL_ADDR,SIZE_DATA,SIZE_X,SIZE_Y>::int_process() {

	irq = false;
	noc_nd = false;

	sc_module::wait();
	while(true){

		noc_nd = nd;
		irq = nd & ~noc_nd;

		sc_module::wait();
	}
}



#endif /* RTSNOC_BUS_PROXY_H_ */
