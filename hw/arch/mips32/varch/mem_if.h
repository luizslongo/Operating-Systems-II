/*
 * mem_if.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef MEM_IF_H_
#define MEM_IF_H_

#include <systemc.h>
#include "plasma_pack.h"

class exec_mem_if : public sc_interface {

public:
	virtual void write(mem_info*, wb_info*, cpu_info*, pc_info*, dec_info*) = 0;
};

#endif /* MEM_IF_H_ */
