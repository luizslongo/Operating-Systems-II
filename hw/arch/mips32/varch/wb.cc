/*
 * wb.cpp
 *
 *  Created on: 04/03/2010
 *      Author: tiago
 */

#include "wb.h"

//#define dec_verbose
//#define r_verbose

/*
 *  interfaces
 */

void wb::write(wb_info *data, cpu_info *status, pc_info* pc_, dec_info* dec_) {

	if(busy)
		wait(free);

	busy = 1;

	if(data)
		memcpy(wb_data, data, sizeof(wb_info));

	memcpy(cpu_status, status, sizeof(cpu_info));
	memcpy(pc_data, pc_, sizeof(pc_info));
	memcpy(dec_data, dec_, sizeof(dec_info));

	operate.notify();
}

/*
 *  process
 */

void wb::do_wb() {

	while(1) {
		busy = 0;
		free.notify();

#ifdef cpu_debug2
		std::cout << "DEBUG - wb::do_wb() - aguardando para novo write back\n";
#endif

		wait(operate);

#ifdef cpu_debug1
		std::cout << "DEBUG - wb::do_wb() - executando write back\n";
#endif

#ifdef dec_verbose
		show_dec();
#endif

		if((cpu_status->skip == 0) && (wb_data)) {
			if(wb_data->un_sig == 1)
				u[wb_data->reg_dest] = wb_data->result;
			else
				r[wb_data->reg_dest] = wb_data->result;
		}
#ifdef r_verbose
		show_r();
#endif

	}
}

/*
 *  internal functions
 */

void wb::show_dec() {

	/*
	 *  show pc and opcode
	 */

	std::cout << "*" << std::setfill('0') << std::setw(8) << std::hex << pc_data->pc_fetch << " " << std::setw(8) << pc_data->opcode;

	/*
	 *  show decoded data
	 */

	std::cout << std::setfill(' ') << std::setw(9);
	if(dec_data->op == 0) std::cout << special_string[dec_data->func];
	else if (dec_data->op == 1) std::cout << regimm_string[dec_data->rt];
	else std::cout << opcode_string[dec_data->op];

	std::cout << " " << std::dec << "$" << std::setfill('0') << std::setw(2) << dec_data->rs;
	std::cout << " " << std::dec << "$" << std::setfill('0') << std::setw(2) << dec_data->rt;
	std::cout << " " << std::dec << "$" << std::setfill('0') << std::setw(2) << dec_data->rd;
	std::cout << " " << std::dec << "$" << std::setfill('0') << std::setw(2) << dec_data->re;
	std::cout << " " << std::setw(4) << std::hex << dec_data->imm;

	std::cout << " r[" << std::dec << std::setfill('0') << std::setw(2) << dec_data->rs << "]=" << std::setw(8) << std::hex << r[dec_data->rs];
	std::cout << " r[" << std::dec << std::setfill('0') << std::setw(2) << dec_data->rt << "]=" << std::setw(8) << std::hex << r[dec_data->rt];

	std::cout << "\n";

}

void wb::show_r() {

	/*
	 *  show value of r[]
	 */

	std::cout << "pid=" << cpu_status->processId << " userMode=" << cpu_status->userMode << ", epc=0x" << std::hex << cpu_status->epc << "\n";

	for(int i = 0; i < 4; ++i) {
		std::cout << std::setfill('0') << std::setw(2) << std::dec << (i * 8) << " ";
		for(int j = 0; j < 8; ++j) {
			std::cout << std::setw(8) << std::hex << r[i*8+j] << " ";
		}
		std::cout << "\n";
	}

}
