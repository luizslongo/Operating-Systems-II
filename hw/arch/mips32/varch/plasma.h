/*
 * plasma.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef PLASMA_H_
#define PLASMA_H_

#include <systemc.h>
#include <arch/common/cpu.h>
#include <arch/common/mem_ctrl_if.h>
#include "fetch.h"
#include "decoder.h"
#include "exec.h"
#include "mem.h"
#include "wb.h"

class plasma : public cpu {

public:

	plasma(sc_module_name nm) : cpu(nm){

		r = new int[32];

		fet = new fetch("fet");
		dec = new decoder("dec", r);
		exe = new exec("exe");
		m = new mem("m");
		w = new wb("w", r);



		fet->decoder(*dec);
		fet->mem_ctrl(bus);
		fet->clk(clk);
		fet->rst(rst);

		dec->exec(*exe);

		exe->fetch(*fet);
		exe->mem(*m);
		exe->irq(irq);
		exe->rst(rst);

		m->mem_ctrl(bus);
		m->wb(*w);

		w->decoder(*dec);

	}

	~plasma() {
		if(r) 	{ delete r; }

		if(fet) {delete fet; fet = (fetch*) 0;}
		if(dec) {delete dec; dec = (decoder*) 0;}
		if(exe) {delete exe; exe = (exec*) 0;}
		if(m)   {delete m; m = (mem*) 0;}
		if(w)	{delete w; w = (wb*) 0;}
	}

#ifdef dump_internal
public:
#else
private:
#endif

	fetch		*fet;
	decoder		*dec;
	exec		*exe;
	mem			*m;
	wb			*w;

	int			*r;

};


#endif /* PLASMA_H_ */
