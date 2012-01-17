/*
 * mem_ctrl.h
 *
 *  Created on: 29/03/2010
 *      Author: tiago
 */

#ifndef MEM_CTRL_H_
#define MEM_CTRL_H_

#include <systemc.h>
#include "module_if.h"
#include "mem_ctrl_if.h"
//#include "plasma_pack.h"

class mem_ctrl : public sc_module,
	public mem_ctrl_if {

public:

	// ports
	sc_port<module_if, 0>	module_port;

	// constructor
	mem_ctrl(sc_module_name nm_) : sc_module(nm_) {

		lock_ = 0;
	}

	// destructor
	~mem_ctrl() {

	}

	//interfaces
	void lock();
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

private:

	void 			end_of_elaboration();
	module_if* 		get_module(unsigned int address);

	sc_event		free;

	int				lock_;

};

#endif /* MEM_CTRL_H_ */
