/*
 * fetch.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include "fetch.h"

/*
 * 	interfaces
 */

void fetch::write(pc_new_info *pc_new_data_) {
	if(busy)
		wait(free);

	busy = 1;

	memcpy(pc_new_data, pc_new_data_, sizeof(pc_new_info));

	operate.notify();
}

/*
 *  process
 */

void fetch::reset_sync() {
	if(rst.read() == true){
		//std::cout << "##RST=true##" << std::endl;
		pc = 0x00;
		pc_data->pc_plus4 = 0x04;
		stop_count = 0;
		reset = true;
	}
	else{
		//std::cout << "##RST=false##" << std::endl;
		reset = false;
		can_go.notify();

	}

}

void fetch::do_fetch() {

	while(1) {

		busy = 0;
		free.notify();

#ifdef cpu_debug2
		std::cout << "DEBUG - fetch::do_fetch() - aguardando para novo fetch\n";
#endif
		wait(operate);

#ifdef cpu_debug1
		std::cout << "DEBUG - fetch::do_fetch() - executando fetch\n";
#endif

		while(reset) wait(can_go);
		wait(clk.posedge_event());

		mem_ctrl->lock();
		mem_ctrl->read(&pc_data->opcode, pc, 4);
		pc_data->pc_fetch = pc;

		//std::cout << "OP_CODE: " << (void*)pc_data->opcode << " fetched from " << (void*)(pc_data->pc_fetch) << "\n";

		switch(pc_new_data->pc_source) {
		case FROM_INC4:
			pc_next = pc_data->pc_plus4;
			break;
		case FROM_BRANCH:
		case FROM_LBRANCH:
			pc_next = pc_new_data->pc_opcode;
			break;
		case FROM_EXCEPTION:
			pc_next = 0x3c;
			break;
		case FROM_REG:
			pc_next = pc_new_data->pc_new;
			break;
		default:
			pc_next = pc_data->pc_plus4;
		}

		pc_data->epc = pc + 4;
		if(pc_next != pc + 4) {
			pc_data->epc |= 2; // branch delay slot
		}

		pc_next &= ~3; // byte align
		pc = pc_next;
		pc_data->pc_plus4 = pc + 4;

		decoder->write(pc_data);

		//if(stop_count == 1300)
		//	return;
		//else
		//	stop_count++;
	}
}
