/*
 * mem_ctrl_if.h
 *
 *  Created on: 29/03/2010
 *      Author: tiago
 */

#ifndef MEM_CTRL_IF_H_
#define MEM_CTRL_IF_H_

#include <systemc.h>

class mem_ctrl_if : public sc_interface {

public:
	virtual void lock() = 0;
	virtual void read(unsigned int *data, unsigned int address, int size) = 0;
	virtual void write(unsigned int data, unsigned int address, int size) = 0;
};


#endif /* MEM_CTRL_IF_H_ */
