/*
 * decoder.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef DECODER_H_
#define DECODER_H_

#include <systemc.h>
#include "plasma_pack.h"
#include "decoder_if.h"
#include "fetch_if.h"
#include "exec_if.h"

class decoder : public sc_module,
	public fetch_decoder_if,
	public wb_decoder_if {

public:

	// ports
	sc_port<decoder_exec_if> exec;

	SC_HAS_PROCESS(decoder);

	// constructor
	decoder(sc_module_name nm, int *r_) :
		sc_module(nm),
		r(r_) {
		// process declaration
		SC_THREAD(do_decode);

		pc_data = new pc_info;
		dec_data = new dec_info;

		u = (unsigned int*) r;

		busy = 0;
	}

	// destructor
	~decoder() {
		if(pc_data) { delete pc_data; pc_data = NULL; }
		if(dec_data) { delete dec_data; dec_data = NULL; }
	}

	// process
	void do_decode();

	// interfaces
	void write(pc_info* pc_data_);
	void write_result(int, unsigned int, int);

private:

	sc_event		operate;
	sc_event		free;

	int				busy;

	int				*r;
	unsigned int	*u;

	pc_info			*pc_data;
	dec_info		*dec_data;
};


#endif /* DECODER_H_ */
