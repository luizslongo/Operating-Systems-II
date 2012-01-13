/*
 * cpu.h
 *
 *  Created on: Jan 13, 2012
 *      Author: tiago
 */

#ifndef CPU_H_
#define CPU_H_

#include <systemc.h>
#include "mem_ctrl_if.h"

SC_MODULE(cpu){
	sc_in<bool>		clk;
	sc_in<bool>		rst;
	sc_in<bool>		irq;
	sc_port<mem_ctrl_if> bus;

	SC_CTOR(cpu){

	}
};

#endif /* CPU_H_ */
