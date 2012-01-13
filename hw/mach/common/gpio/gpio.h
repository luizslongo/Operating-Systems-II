/*
 * uart.h
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <systemc.h>
#include <fstream>
#include <module_if.h>

class gpio : public sc_module,
	public module_if {

public:

	sc_in<bool>		clk;
	sc_in<bool>		rst;

	sc_in<sc_uint<32> > gambi_gpio_in;
	sc_out<sc_uint<32> > gambi_gpio_out;

	gpio(sc_module_name nm_
			, unsigned int start_address_
			, unsigned int end_address_
			, const char *gpio_in_
			, unsigned int gpio_in_initial
			, const char *gpio_out_
			, unsigned int gpio_out_initial)
		: sc_module(nm_)
		, start_address(start_address_)
		, end_address(end_address_)
		, gpio_in(gpio_in_)
		, gpio_out(gpio_out_)
	{
		write(gpio_in_, gpio_in_initial);
		write(gpio_out_, gpio_out_initial);
		//gambi_gpio_in = gpio_in_initial;
		//gambi_gpio_out = gpio_out_initial;
	}

	~gpio() {

	}

	//interfaces
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

	unsigned int get_start_address() const;
	unsigned int get_end_address() const;

private:

	unsigned int	start_address;
	unsigned int	end_address;

	const char *gpio_in;
	const char *gpio_out;

	unsigned int read(const char *filename);
	void write(const char *filename, unsigned int val);

//	sc_uint<32> gambi_gpio_in;
//	sc_uint<32> gambi_gpio_out;

//public:
//	sc_uint<32> get_gpio_in() { return gambi_gpio_in; }
//	sc_uint<32> get_gpio_out() { return gambi_gpio_out; }
//	void set_gpio_in(sc_uint<32> val) { gambi_gpio_in = val; }
//	void set_gpio_out(sc_uint<32> val) { gambi_gpio_out = val; }


};

#endif /* GPIO_H_ */
