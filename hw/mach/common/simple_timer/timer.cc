/*
 * uart.cpp
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include "timer.h"

void timer::read(unsigned int *data, unsigned int address, int size) {

	*data = time;

	//std::cout << "##INFO: timer::read(data=" << (void*)(*data) << " address=" << (void*)address << ") |"
	//		  <<" time=" << time
	//		  <<" int_time=" << int_time
	//		  <<"\n";

	wait(clk.posedge_event());

}

void timer::write(unsigned int data, unsigned int address, int size) {

	increment = data;
	int_time = (data != 0) ? (time + data) : 0;

	//std::cout << "##INFO: timer::write(data=" << (void*)data << " address=" << (void*)address << ") |"
	//			  <<" time=" << time
	//			  <<" int_time=" << int_time
	//			  <<"\n";

	wait(clk.posedge_event());

}

unsigned int timer::get_start_address() const{
	return start_address;
}

unsigned int timer::get_end_address() const{
	return end_address;
}

void timer::timer_proc() {
	time = 0;
	int_time = 0;
	irq = false;

	wait();
	while(true){

		++time;

		if((int_time != 0) && (int_time == time)){
			int_time = time + increment;
			irq = true;
			//std::cout << "##INFO: timer::timer_proc(irq=true) |"
			//				  <<" time=" << time
			//				  <<" int_time=" << int_time
			//				  <<"\n";
		}
		else{
			irq = false;
		}

		wait();
	}
}




