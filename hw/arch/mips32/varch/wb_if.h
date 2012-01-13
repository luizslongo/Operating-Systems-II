/*
 * wb_if.h
 *
 *  Created on: 03/03/2010
 *      Author: tiago
 */

#ifndef WB_IF_H_
#define WB_IF_H_

#include <systemc.h>
#include "plasma_pack.h"

class wb_if : public sc_interface {

public:
	virtual void write(wb_info*, cpu_info*, pc_info*, dec_info*) = 0;
};

#endif /* WB_IF_H_ */
