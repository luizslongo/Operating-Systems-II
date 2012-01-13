/*
 * uart.cpp
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include "pic.h"

void pic::read(unsigned int *data, unsigned int address, int size) {

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case 0:
		*data = edge_enable;
		break;
	case 1:
		*data = polarity;
		break;
	case 2:
		*data = mask;
		break;
	case 3:
		*data = pending;
		break;
	default:
		*data = 0;
		break;
	}

	//std::cout << "##INFO: pic::read(data=" << (void*)(*data) << " address=" << (void*)address << ")\n"
	//			  <<"\tedge_enable=\t\t" << edge_enable.to_string(SC_BIN)<<"\n"
	//			  <<"\tpolarity=\t\t" << polarity.to_string(SC_BIN)<<"\n"
	//			  <<"\tmask=\t\t\t" << mask.to_string(SC_BIN)<<"\n"
	//			  <<"\tpending=\t\t" << pending.to_string(SC_BIN)
	//			  <<"\n";

	wait(clk.posedge_event());

}

void pic::write(unsigned int data, unsigned int address, int size) {

	unsigned int addr = (address & 0x000000FF) >> 2;

	switch (addr) {
	case 0:
		edge_enable = data;
		break;
	case 1:
		polarity = data;
		break;
	case 2:
		mask = data;
		break;
	case 3:{
		pending &= ~data;
		break;
	}
	default:
		break;
	}

	//std::cout << "##INFO: pic::write(data=" << (void*)data << " address=" << (void*)address << ")\n"
	//			  <<"\tedge_enable=\t\t" << edge_enable.to_string(SC_BIN)<<"\n"
	//			  <<"\tpolarity=\t\t" << polarity.to_string(SC_BIN)<<"\n"
	//			  <<"\tmask=\t\t\t" << mask.to_string(SC_BIN)<<"\n"
	//			  <<"\tpending=\t\t" << pending.to_string(SC_BIN)
	//			  <<"\n";

	wait(clk.posedge_event());

}

unsigned int pic::get_start_address() const{
	return start_address;
}

unsigned int pic::get_end_address() const{
	return end_address;
}

void pic::pic_proc() {
	polarity = 0;
	edge_enable = 0;
	mask = 0;

	for (int i = 0; i < 32; ++i) {
		irq_in_prev[i] = false;
	}

	irq_out = false;

	wait();
	while(true){

		bool current[32];
		for (int i = 0; i < 32; ++i) {
			current[i] = irq_in[i];
		}


		sc_uint<32> pending_prev = pending;

		for (int i = 0; i < 32; ++i) {
			if(edge_enable[i]){
				if(polarity[i])	pending[i] |= current[i] && !irq_in_prev[i];
				else 			pending[i] |= !current[i] && irq_in_prev[i];
			}
			else{
				pending[i] |= polarity[i] ? current[i] : !current[i];
			}
		}


		for (int i = 0; i < 32; ++i) {
			irq_in_prev[i] = current[i];
		}

		sc_uint<32> irq = pending & (~mask);
		irq_out = irq.or_reduce();

		//pending_prev[1] = pending[1];
		//pending_prev[2] = pending[2];
		//if(pending_prev != pending)
		//		std::cout << "##INFO: pic::pic_proc(interrupt pending)\n"
		//			  <<"\tedge_enable=\t\t" << edge_enable.to_string(SC_BIN)<<"\n"
		//			  <<"\tpolarity=\t\t" << polarity.to_string(SC_BIN)<<"\n"
		//			  <<"\tmask=\t\t\t" << mask.to_string(SC_BIN)<<"\n"
		//			  <<"\tpending=\t\t" << pending.to_string(SC_BIN)
		//			  <<"\n";

		wait();
	}
}




