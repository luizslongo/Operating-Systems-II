/*
 * exec_if.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef EXEC_IF_H_
#define EXEC_IF_H_

#include <systemc.h>
#include "plasma_pack.h"

class decoder_exec_if : public sc_interface {

public:
	virtual void write(pc_info*, dec_info*, int, int) = 0;
};

#endif /* EXEC_IF_H_ */
