/*
 * wb.h
 *
 *  Created on: 04/03/2010
 *      Author: tiago
 */

#ifndef WB_H_
#define WB_H_

#include <systemc.h>
#include "wb_if.h"
#include "decoder_if.h"

class wb : public sc_module,
	public wb_if {

public:
	// ports
	sc_port<wb_decoder_if> decoder;

	SC_HAS_PROCESS(wb);

	// constructor
	wb(sc_module_name nm, int *r_) :
		sc_module(nm),
		r(r_){

		//process declaration
		SC_THREAD(do_wb);

		pc_data = new pc_info;
		dec_data = new dec_info;
		wb_data = new wb_info;
		cpu_status = new cpu_info;

		u = (unsigned int*) r;

		busy = 0;
	}

	// destructor
	~wb() {
		if(pc_data) { delete pc_data; pc_data = NULL; }
		if(dec_data) { delete dec_data; dec_data = NULL; }
		if(wb_data) { delete wb_data; wb_data = NULL; }
		if(cpu_status) { delete cpu_status; cpu_status = NULL; }
	}

	// process
	void do_wb();

	// interfaces
	void write(wb_info*, cpu_info*, pc_info*, dec_info*);

private:

	void			show_dec();
	void			show_r();

	sc_event		operate;
	sc_event		free;

	int				busy;

	pc_info			*pc_data;
	dec_info		*dec_data;
	wb_info			*wb_data;
	cpu_info		*cpu_status;

	int				*r;
	unsigned int	*u;

};

#endif /* WB_H_ */
