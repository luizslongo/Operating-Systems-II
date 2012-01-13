/*
 * uart.cpp
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include "uart.h"
#include "plasma_pack.h"

//#define uart_debug

void uart::read(unsigned int *data, unsigned int address, int size) {

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case SUART_TXLEVEL:
		*data = (unsigned int)ntohl((32 - tx_fifo.size()));
		break;
	case SUART_RXLEVEL:
		*data = (unsigned int)ntohl(rx_fifo.size());
		break;
	case SUART_RXCHAR:
		if(!rx_fifo.empty()){
			*data = (unsigned int)ntohl(rx_fifo.front());
			rx_fifo.pop();
			//std::cout << "uart::read():" << data->to_string(SC_HEX) << "\n";
		}
		else
			*data = 0;
		break;
	default:
		*data = 0;
		break;
	}

	wait(clk.posedge_event());

}

void uart::write(unsigned int data, unsigned int address, int size) {

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case SUART_CLKDIV:
		//TODO
		break;
	case SUART_TXCHAR:
		tx_fifo.push((unsigned int)htonl(data));
		tx_fifo_add.notify();
		break;
	default:
		break;
	}

	wait(clk.posedge_event());

}

unsigned int uart::get_start_address() const{
	return start_address;
}

unsigned int uart::get_end_address() const{
	return end_address;
}

void uart::tx_thread(){
	while(true){
		wait(tx_fifo_add);

		while(!tx_fifo.empty()){
			unsigned int val = tx_fifo.front();
			tx_fifo.pop();

			//std::cout << std::setfill('0') << std::setw(8) << std::hex << data << "\n";

			//std::cout << (char) htonl(val);

			pty.terminal_write((char) htonl(val));
		}
	}

}

void uart::rx_thread(){
	while(true){
		if(pty.input_available()){
			char c = pty.terminal_read();
			rx_fifo.push(htonl(0x000000FF & (unsigned int)c));
		}
		wait(clk.posedge_event());
	}
}

void uart::rxtx_int(){
	tx_int = (32 - tx_fifo.size()) != 0;
	rx_int = !rx_fifo.empty();
	wait();
	while(true){
		tx_int = (32 - tx_fifo.size()) != 0;
		rx_int = !rx_fifo.empty();
		wait();
	}
}


