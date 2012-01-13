/*
 * exec.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include "plasma_pack.h"
#include "exec.h"

/*
 *  interfaces
 */

void exec::write(pc_info* pc_, dec_info* dec_, int A_, int B_) {

#ifdef cpu_debug3
	std::cout << "DEBUG - exec::write - recebendo dados do decoder\n";
#endif

	if(busy)
		wait(free);

#ifdef cpu_debug3
	std::cout << "DEBUG - exec::write - decoder livre\n";
#endif

	busy = 1;

	memcpy(pc_data, pc_, sizeof(pc_info));
	memcpy(dec_data, dec_, sizeof(dec_info));

	A = A_;
	B = B_;

	uB = (unsigned int) B;
	uA = (unsigned int) A;

#ifdef cpu_debug3
	std::cout << "DEBUG - exec::write - dados copiados\n";
#endif

	operate.notify();

#ifdef cpu_debug3
	std::cout << "DEBUG - exec::write - operate.notify()\n";
#endif
}

/*
 *  process
 */

void exec::do_exec() {

	while(1) {

		fetch->write(pc_new_data);
		busy = 0;
		free.notify();

#ifdef cpu_debug2
		std::cout << "DEBUG - exec::do_exec() - aguardando nova operacao\n";
#endif

		wait(operate);

#ifdef cpu_debug1
		std::cout << "DEBUG - exec::do_exec() - executando operacao\n";
#endif


		branch = 0;
		lbranch = 2;

		pc_new_data->pc_source = FROM_INC4;
		pc_new_data->pc_opcode = dec_data->imm_shift + pc_data->pc_plus4;


		if(mem_data) { delete mem_data; mem_data = NULL; }
		if(wb_data) { delete wb_data; wb_data = NULL; }

		if(cpu_status->skip == 0) {

			do_operate();

			cpu_status->skip = (lbranch == 0);

			if(branch || lbranch == 1)
				pc_new_data->pc_source = FROM_BRANCH;

			if(exceptionId) {
				// restaura o valor de RT - necessário?
				wb_data = new wb_info;
				wb_data->result = B;
				wb_data->reg_dest = dec_data->rt;


				pc_new_data->pc_source = FROM_EXCEPTION;
				cpu_status->epc = epc;
				cpu_status->skip = 1;
				exceptionId = 0;
			}
		} else {
			cpu_status->skip = 0; // não realiza a próxima instrução
		}
		mem->write(mem_data, wb_data, cpu_status, pc_data, dec_data);
	}
}

/*
 *  internal functions
 */

void exec::do_operate() {

	bool donot_handle_irq_now = false;

	switch(dec_data->op) {
	case SPECIAL:
		switch(dec_data->func) {
		case SLL:
			wb_data = new wb_info;
			wb_data->result = B << dec_data->re;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SRL:
			wb_data = new wb_info;
			wb_data->result = uB >> dec_data->re;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SRA:
			wb_data = new wb_info;
			wb_data->result = B >> dec_data->re;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SLLV:
			wb_data = new wb_info;
			wb_data->result = B << A;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SRLV:
			wb_data = new wb_info;
			wb_data->result = uB >> A;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SRAV:
			wb_data = new wb_info;
			wb_data->result = B >> A;
			wb_data->reg_dest = dec_data->rd;
			break;
		case JR:
			//std::cout << "JR: " << std::setfill('0') << std::setw(8) << std::hex << A << "\n";
			pc_new_data->pc_new = A;
			pc_new_data->pc_source = FROM_REG;
			donot_handle_irq_now = true;
			break;
		case JALR:
			wb_data = new wb_info;
			wb_data->result = pc_data->pc_plus4;
			wb_data->reg_dest = dec_data->rd;
			pc_new_data->pc_new = A;
			pc_new_data->pc_source = FROM_REG;
			donot_handle_irq_now = true;
			break;
		case MOVZ: // não consta nos opcodes
			if(!B) {
				wb_data = new wb_info;
				wb_data->result = A;
				wb_data->reg_dest = dec_data->rd;
			}
			break;  /*IV*/
		case MOVN: // não consta nos opcodes
			if(B) {
				wb_data = new wb_info;
				wb_data->result = A;
				wb_data->reg_dest = dec_data->rd;
			}
			break;  /*IV*/
		case SYSCALL:
			if(status & 1){
				epc = pc_data->epc | 1;
				exceptionId = 1;
			}
			break;
		case BREAK:
			if(status & 1){
				epc = pc_data->epc | 1;
				exceptionId = 1;
			}
			break;
		case SYNC: break; // não consta nos opcodes
		case MFHI:
			wb_data = new wb_info;
			wb_data->result = hi;
			wb_data->reg_dest = dec_data->rd;
			break;
		case MTHI:
			hi = A;
			break;
		case MFLO:
			wb_data = new wb_info;
			wb_data->result = lo;
			wb_data->reg_dest = dec_data->rd;
			break;
		case MTLO:
			lo = A;
			break;
		case MULT:
			mult_big_signed(A, B);
			break;
		case MULTU:
			mult_big(A, B);
			break;
		case DIV:
			lo = A / B;
			hi = A % B;
			break;
		case DIVU:
			lo = uA / uB;
			hi = uA % uB;
			break;
		case ADD:
			wb_data = new wb_info;
			wb_data->result = A + B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case ADDU:
			wb_data = new wb_info;
			wb_data->result = A + B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SUB:
			wb_data = new wb_info;
			wb_data->result = A - B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SUBU: // não consta nos opcodes
			wb_data = new wb_info;
			wb_data->result = A - B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case AND:
			wb_data = new wb_info;
			wb_data->result = A & B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case OR:
			wb_data = new wb_info;
			wb_data->result = A | B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case XOR:
			wb_data = new wb_info;
			wb_data->result = A ^ B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case NOR:
			wb_data = new wb_info;
			wb_data->result = ~(A | B);
			wb_data->reg_dest = dec_data->rd;
			break;
		case SLT:
			wb_data = new wb_info;
			wb_data->result = A < B;
			wb_data->reg_dest = dec_data->rd;
			break;
		case SLTU: // não consta nos opcodes
			wb_data = new wb_info;
			wb_data->result = uA < uB;
			wb_data->reg_dest = dec_data->rd;
			break;
		case DADDU: // não consta nos opcodes
			wb_data = new wb_info;
			wb_data->result = A + uB;
			wb_data->reg_dest = dec_data->rd;
			break;
		case TGEU:	std::cout << "##ERROR - Instruction TGEU not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
		case TLT:	std::cout << "##ERROR - Instruction TLT not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
		case TLTU:  std::cout << "##ERROR - Instruction TLTU not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
		case TEQ:	std::cout << "##ERROR - Instruction TEQ not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
		case TNE:	std::cout << "##ERROR - Instruction TNE not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
		default:	std::cout << "##ERROR - Unknown instruction opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
		}
		break;
	case REGIMM:
		switch(dec_data->rt) {
		case BLTZAL:
			wb_data = new wb_info;
			wb_data->result = pc_data->pc_plus4;
			wb_data->reg_dest = 31;
		case BLTZ:		branch = A < 0;		break;
		case BGEZAL: // não consta nos opcodes
			wb_data = new wb_info;
			wb_data->result = pc_data->pc_plus4;
			wb_data->reg_dest = 31;
		case BGEZ:		branch = A >= 0;	break; // não consta nos opcodes
		case BLTZALL: // não consta nos opcodes
			wb_data = new wb_info;
			wb_data->result = pc_data->pc_plus4;
			wb_data->reg_dest = 31;
		case BLTZL:		lbranch = A < 0;	break; // não consta nos opcodes
		case BGEZALL: // não consta nos opcodes
			wb_data = new wb_info;
			wb_data->result = pc_data->pc_plus4;
			wb_data->reg_dest = 31;
		case BGEZL: 	lbranch = A >= 0;	break; // não consta nos opcodes
		default: 	std::cout << "##ERROR - Unknown instruction opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
		}
		break;
	case JAL:
		wb_data = new wb_info;
		wb_data->result = pc_data->pc_plus4;
		wb_data->reg_dest = 31;
	case J:
		pc_new_data->pc_new = ((pc_data->pc_plus4 - 4) & 0xf0000000) | dec_data->target;
		pc_new_data->pc_source = FROM_REG;
		donot_handle_irq_now = true;
		break;
	case BEQ:	branch = A == B;	break;
	case BNE:	branch = A != B;	break;
	case BLEZ:	branch = A <= 0;	break;
	case BGTZ:	branch = A > 0;		break;
	case ADDI:
		wb_data = new wb_info;
		wb_data->result = A + (short)dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case ADDIU:
		wb_data = new wb_info;
		wb_data->result = uA + (short)dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		wb_data->un_sig = 1;
		break;
	case SLTI:
		wb_data = new wb_info;
		wb_data->result = A < (short)dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case SLTIU:
		wb_data = new wb_info;
		wb_data->result = uA < (unsigned int)(short)dec_data->imm;
		wb_data->un_sig = 1;
		wb_data->reg_dest = dec_data->rt;
		break;
	case ANDI:
		wb_data = new wb_info;
		wb_data->result = A & dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case ORI:
		wb_data = new wb_info;
		wb_data->result = A | dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case XORI:
		wb_data = new wb_info;
		wb_data->result = A ^ dec_data->imm;
		wb_data->reg_dest = dec_data->rt;
		break;
	case LUI:
		wb_data = new wb_info;
		wb_data->result = (dec_data->imm << 16);
		wb_data->reg_dest = dec_data->rt;
		break;
	case COP0:
		if((pc_data->opcode & (1 << 23)) == 0) {// move from CP0
			wb_data = new wb_info;
			if(dec_data->rd == 12){
				wb_data->result = status; //std::cout << "##INFO move from CP0 - status = " << status << "\n";
			}
			else if(dec_data->rd == 13){
				std::cout << "##ERROR move from CP0 - cause reg not implemented " << dec_data->rd << "\n"; while(true);
			}
			else if(dec_data->rd == 14)
				wb_data->result = cpu_status->epc;
			else{
				std::cout << "##ERROR move from CP0 - unknown cp0 reg " << dec_data->rd << "\n"; while(true);
			}
			wb_data->reg_dest = dec_data->rt;
		} else {
			if(dec_data->rd == 12){
				status = B; //std::cout << "##INFO move to CP0 - status = " << status << "\n";
				if(cpu_status->processId && (B & 2)) {
					cpu_status->userMode |= B & 2;
				}
			}
			else if(dec_data->rd == 13){
				std::cout << "##ERROR move to CP0 - cause reg not implemented " << dec_data->rd << "\n"; while(true);
			}
			else if(dec_data->rd == 14)
				cpu_status->epc = B;
			else{
				std::cout << "##ERROR move to CP0 - unknown cp0 reg " << dec_data->rd << "\n"; while(true);
			}

		}
		break;
	case BEQL:	lbranch = A == B;			break; // não consta nos opcodes
	case BNEL:	lbranch = A != B;			break; // não consta nos opcodes
	case BLEZL:	lbranch = A <= 0;			break; // não consta nos opcodes
	case BGTZL: lbranch = A > 0;			break; // não consta nos opcodes
	case LB:
		mem_data = new mem_info;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 1;
		mem_data->reg_dest = dec_data->rt;
		break;
	case LH:
		mem_data = new mem_info;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 2;
		mem_data->reg_dest = dec_data->rt;
		break;
	case LWL:	//break;
	case LW:
		mem_data = new mem_info;
		mem_data->address = (short)dec_data->imm + A;
		//std::cout << "Instrução LW - endereço para leitura:" << std::setfill(' ') << std::setw(8) << std::hex << mem_data->address << "\n";
		mem_data->size = 4;
		mem_data->reg_dest = dec_data->rt;
		break;
	case LBU: // não consta nos opcodes
		mem_data = new mem_info;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 1;
		mem_data->reg_dest = dec_data->rt;
		mem_data->un_sig = 1;
		break;
	case LHU: // não consta nos opcodes
		mem_data = new mem_info;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 2;
		mem_data->reg_dest = dec_data->rt;
		mem_data->un_sig = 1;
		break;
	case LWR:	std::cout << "##ERROR - Instruction LWR not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
	case SB:
		mem_data = new mem_info;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 1;
		mem_data->mem_r_w = 0;
		break;
	case SH:
		mem_data = new mem_info;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 2;
		mem_data->mem_r_w = 0;
		break;
	case SWL:	//break;
	case SW:
		mem_data = new mem_info;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 4;
		mem_data->mem_r_w = 0;
		break;
	case SWR:	std::cout << "##ERROR - Instruction SWR not implemented opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
	case CACHE: break; // não consta nos opcodes
	case LL: // não consta nos opcodes
		mem_data = new mem_info;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 4;
		mem_data->reg_dest = dec_data->rt;
		break;
	case SC: // não consta nos opcodes
		mem_data = new mem_info;
		mem_data->data = B;
		mem_data->address = (short)dec_data->imm + A;
		mem_data->size = 4;
		mem_data->mem_r_w = 0;
		wb_data = new wb_info;
		wb_data->reg_dest = dec_data->rt;
		wb_data->result = 1;
		break;
	default:	std::cout << "##ERROR - Unknown instruction opcode: " << dec_data->op << " func: " << dec_data->func << "\n"; while(true);
	}

	if(irq.read() && !rst.read() && !donot_handle_irq_now){
		if(status & 1){
			epc = pc_data->epc | 1;
			exceptionId = 1;
			//std::cout << "##INFO: interrupt triggered\n";
			status = status & ~1;
		}
		//else{
		//	std::cout << "##INFO: interrupt triggered and ignored\n";
		//}
	}
}

void exec::mult_big(int a, int b) {

	unsigned int ahi, alo, bhi, blo;
	unsigned int c0, c1, c2;
	unsigned int c1_a, c1_b;

	ahi = (unsigned int)a >> 16;
	alo = a & 0xffff;
	bhi = (unsigned int)b >> 16;
	blo = b & 0xffff;

	c0 = alo * blo;
	c1_a = ahi * blo;
	c1_b = alo * bhi;
	c2 = ahi * bhi;

	c2 += (c1_a >> 16) + (c1_b >> 16);
	c1 = (c1_a & 0xffff) + (c1_b & 0xffff) + (c0 >> 16);
	c2 += (c1 >> 16);
	c0 = (c1 << 16) + (c0 & 0xffff);

	hi = c2;
	lo = c0;
}

void exec::mult_big_signed(int a, int b) {

	unsigned int ahi, alo, bhi, blo;
	unsigned int c0, c1, c2;
	unsigned int c1_a, c1_b;

	ahi = a >> 16;
	alo = a & 0xffff;
	bhi = b >> 16;
	blo = b & 0xffff;

	c0 = alo * blo;
	c1_a = ahi * blo;
	c1_b = alo * bhi;
	c2 = ahi * bhi;

	c2 += (c1_a >> 16) + (c1_b >> 16);
	c1 = (c1_a & 0xffff) + (c1_b & 0xffff) + (c0 >> 16);
	c2 += (c1 >> 16);
	c0 = (c1 << 16) + (c0 & 0xffff);
	hi = c2;
	lo = c0;
}
