/*
 * uart.h
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#ifndef PIC_H_
#define PIC_H_

#include <systemc.h>
#include <arch/common/module_if.h>

class pic : public sc_module,
	public module_if {

public:

	sc_in<bool>		clk;
	sc_in<bool>		rst;
	sc_in<bool>		irq_in[32];
	sc_out<bool>	irq_out;

	SC_HAS_PROCESS(pic);
	pic(sc_module_name nm_
			, unsigned int start_address_
			, unsigned int end_address_
			)
		: sc_module(nm_)
		, start_address(start_address_)
		, end_address(end_address_)
	{

		SC_CTHREAD(pic_proc, clk.pos());
		reset_signal_is(rst, true);
	}

	~pic() {

	}

	//interfaces
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

	unsigned int get_start_address() const;
	unsigned int get_end_address() const;

private:

	unsigned int	start_address;
	unsigned int	end_address;

	void pic_proc();

	sc_uint<32> edge_enable;
	sc_uint<32> polarity;
	sc_uint<32> mask;
	sc_uint<32> pending;

	bool		irq_in_prev[32];




};

#endif /* GPIO_H_ */
