/*
 * module_if.h
 *
 *  Created on: 29/03/2010
 *      Author: tiago
 */

#ifndef MODULE_IF_H_
#define MODULE_IF_H_

#include <systemc.h>

class module_if : public sc_interface {

public:

	virtual void read(unsigned int *data, unsigned int address, int size) = 0;
	virtual void write(unsigned int data, unsigned int address, int size) = 0;

	virtual unsigned int get_start_address() const = 0;
	virtual unsigned int get_end_address() const = 0;
};

#endif /* MODULE_IF_H_ */
