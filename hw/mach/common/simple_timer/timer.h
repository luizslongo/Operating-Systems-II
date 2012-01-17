/*
 * uart.h
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <systemc.h>
#include <arch/common/module_if.h>

class timer : public sc_module,
	public module_if {

public:

	sc_in<bool>		clk;
	sc_in<bool>		rst;
	sc_out<bool>	irq;

	SC_HAS_PROCESS(timer);
	timer(sc_module_name nm_
			, unsigned int start_address_
			, unsigned int end_address_
			)
		: sc_module(nm_)
		, start_address(start_address_)
		, end_address(end_address_)
	{
		time = 0;
		int_time = 0;

		SC_CTHREAD(timer_proc, clk.pos());
		reset_signal_is(rst, true);
	}

	~timer() {

	}

	//interfaces
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

	unsigned int get_start_address() const;
	unsigned int get_end_address() const;

	unsigned long int get_tsc(){ return time;}

private:

	unsigned int	start_address;
	unsigned int	end_address;

	void timer_proc();

	unsigned long int	time;
	unsigned long int	int_time;
	unsigned long int	increment;


};

#endif /* GPIO_H_ */
