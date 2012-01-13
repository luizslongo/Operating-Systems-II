/*
 * exec.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef EXEC_H_
#define EXEC_H_

#include <systemc.h>
#include "fetch_if.h"
#include "exec_if.h"
#include "mem_if.h"

class exec : public sc_module,
	public decoder_exec_if {

public:

	// ports
	sc_in<bool> irq;
	sc_in<bool> rst;

	sc_port<exec_fetch_if> fetch;
	sc_port<exec_mem_if> mem;

	SC_HAS_PROCESS(exec);

	// constructor
	exec(sc_module_name nm) : sc_module(nm) {
		// process declaration
		SC_THREAD(do_exec);

		pc_data = new pc_info;
		pc_new_data = new pc_new_info;
		dec_data = new dec_info;
		cpu_status = new cpu_info;

		busy = 0;

		mem_data = NULL;
		wb_data = NULL;

		pc_new_data->pc_source = FROM_INC4;
		pc_new_data->pc_new = 0x04;
		pc_new_data->pc_opcode = 0x04;

		status = 0;

	}

	// destructor
	~exec() {
		if(pc_data) { delete pc_data; pc_data = NULL; }
		if(pc_new_data) { delete pc_new_data; pc_new_data = NULL; }
		if(dec_data) { delete dec_data; dec_data = NULL; }
		if(cpu_status) { delete cpu_status; cpu_status = NULL; }
	}

	// process
	void do_exec();

	void irq_handle();

	// interfaces
	void write(pc_info*, dec_info*, int, int);

#ifdef dump_internal
public:
#else
private:
#endif

	void			do_operate();
	void			mult_big(int, int);
	void 			mult_big_signed(int, int);

	sc_event		operate;
	sc_event		free;

	int				busy;

	int				A;
	int				B;

	unsigned int	uB;
	unsigned int	uA;

	int				hi;
	int				lo;
	unsigned int	status;

	int				exceptionId;
	int 			branch, lbranch;

	unsigned int	epc;

	pc_info			*pc_data;
	pc_new_info		*pc_new_data;
	dec_info		*dec_data;
	mem_info		*mem_data;
	wb_info			*wb_data;
	cpu_info		*cpu_status;

};

#endif /* EXEC_H_ */
