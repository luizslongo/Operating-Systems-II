/*
 * mem.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef MEM_H_
#define MEM_H_

#include <systemc.h>
#include "mem_if.h"
#include "mem_ctrl_if.h"
#include "wb_if.h"

//#include "memory.h"

class mem : public sc_module,
	public exec_mem_if {

public:

	// ports
	sc_port<mem_ctrl_if> mem_ctrl;
	sc_port<wb_if> wb;

	SC_HAS_PROCESS(mem);

	// constructor
	mem(sc_module_name nm) : sc_module(nm) {
		// process declaration
		SC_THREAD(do_run);

		pc_data = new pc_info;
		dec_data = new dec_info;
		cpu_status = new cpu_info;

		busy = 0;
	}

	// destructor
	~mem() {
		if(pc_data) { delete pc_data; pc_data = NULL; }
		if(dec_data) { delete dec_data; dec_data = NULL; }
		if(cpu_status) { delete cpu_status; cpu_status = NULL; }
	}

	// process
	void do_run();

	// interfaces
	void write(mem_info*, wb_info*, cpu_info*, pc_info*, dec_info*);

private:

	void 			do_mem_access();
	void			do_read();
	void 			do_write();

	sc_event		operate;

	sc_event		free;

	int				busy;

	pc_info			*pc_data;
	dec_info		*dec_data;
	mem_info		*mem_data;
	wb_info			*wb_data;
	cpu_info		*cpu_status;

};

#endif /* MEM_H_ */
