/*
 * fetch_if.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef FETCH_IF_H_
#define FETCH_IF_H_

#include <systemc.h>
#include "plasma_pack.h"

class exec_fetch_if : public sc_interface {

public:
	virtual void write(pc_new_info*) = 0;
};

#endif /* FETCH_IF_H_ */
