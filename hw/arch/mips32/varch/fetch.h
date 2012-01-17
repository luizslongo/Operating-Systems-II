/*
 * fetch.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef FETCH_H_
#define FETCH_H_

#include <systemc.h>
#include <arch/common/mem_ctrl_if.h>
#include "fetch_if.h"
#include "decoder_if.h"
#include "plasma_pack.h"


class fetch : public sc_module,
	public exec_fetch_if {

public:

	// ports
	sc_in<bool>		clk;
	sc_in<bool>		rst;
	sc_port<fetch_decoder_if> decoder;
	sc_port<mem_ctrl_if> mem_ctrl;

	SC_HAS_PROCESS(fetch);

	// constructor
	fetch(sc_module_name nm) : sc_module(nm) {
		// process declaration
		SC_THREAD(do_fetch);

		SC_METHOD(reset_sync);
		sensitive << rst;

		pc_data	= new pc_info;
		pc_new_data = new pc_new_info;

		pc = 0x00;
		pc_data->pc_plus4 = 0x04;

		stop_count = 0;

		reset = true;
	}

	// destructor
	~fetch() {
		if(pc_data) { delete pc_data; pc_data = NULL; }
		if(pc_new_data) { delete pc_new_data; pc_new_data = NULL; }
	}

	// process
	void do_fetch();
	void reset_sync();

	// interfaces
	void write(pc_new_info *);

#ifdef dump_internal
public:
#else
private:
#endif

	sc_event		operate;
	sc_event		free;

	int				busy;

	int 			pc;
	int				pc_next;

	pc_info			*pc_data;
	pc_new_info		*pc_new_data;

	int 			stop_count;

	bool 			reset;
	sc_event		can_go;
};

#endif /* FETCH_H_ */
