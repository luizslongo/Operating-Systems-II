/*
 * router.h
 *
 *  Created on: Jan 12, 2012
 *      Author: tiago
 */

#ifndef RTSNOC_ROUTER_H_
#define RTSNOC_ROUTER_H_

#include <systemc.h>
#include <queue>

template<
	unsigned int X,
	unsigned int Y,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
SC_MODULE(rtsnoc_router) {
	enum{
		BUS_SIZE = SIZE_DATA+(2*SIZE_X)+(2*SIZE_Y)+6,

		ROUTER_NN = 0,
		ROUTER_NE = 1,
		ROUTER_EE = 2,
		ROUTER_SE = 3,
		ROUTER_SS = 4,
		ROUTER_SW = 5,
		ROUTER_WW = 6,
		ROUTER_NW = 7,

		NPORTS = 8
	};

	sc_in<bool> clk;
	sc_in<bool> rst;

	sc_in<sc_uint<BUS_SIZE> > din[NPORTS];
	sc_out<sc_uint<BUS_SIZE> > dout[NPORTS];
	sc_in<bool> wr[NPORTS];
	sc_in<bool> rd[NPORTS];
	sc_out<bool> wait[NPORTS];
	sc_out<bool> nd[NPORTS];


	SC_CTOR(rtsnoc_router){
		SC_CTHREAD(tx_process, clk.pos());
		reset_signal_is(rst, true);

		SC_CTHREAD(rx_process, clk.pos());
		reset_signal_is(rst, true);
	}

private:
	void tx_process();
	void handle_tx(unsigned int port);

	void rx_process();
	void handle_rx(unsigned int port);

	std::queue<sc_uint<BUS_SIZE> > queue[NPORTS];

	unsigned int get_local_dest_addr(const sc_uint<BUS_SIZE> &data) const;

};


template<
	unsigned int X,
	unsigned int Y,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
inline void rtsnoc_router<X,Y,SIZE_DATA,SIZE_X,SIZE_Y>::tx_process(){
	for (int i = 0; i < NPORTS; ++i) {
		nd[i] = false;
		dout[i] = 0;
	}
	sc_module::wait();
	while(true){
		for (int i = 0; i < NPORTS; ++i) handle_tx(i);
		sc_module::wait();
	}
}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
void rtsnoc_router<X,Y,SIZE_DATA,SIZE_X,SIZE_Y>::rx_process(){
	for (int i = 0; i < NPORTS; ++i) {
		wait[i] = false;
	}
	sc_module::wait();
	while(true){
		for (int i = 0; i < NPORTS; ++i) handle_rx(i);
		sc_module::wait();
	}
}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
void rtsnoc_router<X,Y,SIZE_DATA,SIZE_X,SIZE_Y>::handle_tx(unsigned int port){
	nd[port] = !queue[port].empty();
	dout[port] = queue[port].empty() ? sc_uint<BUS_SIZE>(0) : queue[port].front();

	if(rd[port].read() && !queue[port].empty())
		queue[port].pop();

}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
void rtsnoc_router<X,Y,SIZE_DATA,SIZE_X,SIZE_Y>::handle_rx(unsigned int port){
	if(wr[port].read()){
		wait[port] = true;
		queue[get_local_dest_addr(din[port].read())].push(din[port].read());
	}
	else{
		wait[port] = false;
	}
}

template<
	unsigned int X,
	unsigned int Y,
	unsigned int SIZE_DATA,
	unsigned int SIZE_X,
	unsigned int SIZE_Y
>
unsigned int rtsnoc_router<X,Y,SIZE_DATA,SIZE_X,SIZE_Y>::get_local_dest_addr(const sc_uint<BUS_SIZE> &data) const{
	return data.range(SIZE_DATA+2,SIZE_DATA).to_uint();
}



#endif /* ROUTER_H_ */
