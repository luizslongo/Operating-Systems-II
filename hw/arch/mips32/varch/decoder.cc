/*
 * decoder.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include "plasma_pack.h"
#include "decoder.h"

/*
 *  interface
 */

void decoder::write(pc_info *pc_) {

	if(busy)
		wait(free);
	busy = 1;

	memcpy(pc_data, pc_, sizeof(pc_info));

	operate.notify();
}

void decoder::write_result(int result, unsigned int reg, int un_sig) {

	if(un_sig == 1)
		u[reg] = result;
	else
		r[reg] = result;
}

/*
 * 	process
 */

void decoder::do_decode() {

	while(1) {

		busy = 0;
		free.notify();

#ifdef cpu_debug2
		std::cout << "DEBUG - decoder::do_decode() - aguardando para nova decodificacao\n";
#endif

		wait(operate);

#ifdef cpu_debug1
		std::cout << "DEBUG - decoder::do_decode() - executando decodificacao\n";
#endif


		dec_data->op = (pc_data->opcode >> 26) & 0x3f;
		dec_data->rs = (pc_data->opcode >> 21) & 0x1f;
		dec_data->rt = (pc_data->opcode >> 16) & 0x1f;
		dec_data->rd = (pc_data->opcode >> 11) & 0x1f;
		dec_data->re = (pc_data->opcode >> 6) & 0x1f;
		dec_data->func = (pc_data->opcode & 0x3f);
		dec_data->imm = pc_data->opcode & 0xffff;
		dec_data->imm_shift = ((long int)(short int)dec_data->imm << 2) - 4;
		dec_data->target = (pc_data->opcode << 6) >> 4;

		r[0] = 0;

#ifdef cpu_debug3
		std::cout << "DEBUG - decoder::do_decode() - chamando exec->write()\n";
#endif

		exec->write(pc_data, dec_data, r[dec_data->rs], r[dec_data->rt]);
	}
}
