/*
 * decoder_if.h
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#ifndef DECODER_IF_H_
#define DECODER_IF_H_

#include <systemc.h>
#include "plasma_pack.h"

class fetch_decoder_if : public sc_interface {

public:
	virtual void write(pc_info*) = 0;
};

class wb_decoder_if : public sc_interface {

public:
	virtual void write_result(int, unsigned int, int) = 0;
};

#endif /* DECODER_IF_H_ */
