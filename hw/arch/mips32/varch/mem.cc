/*
 * mem.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include "mem.h"

/*
 *  interfaces
 */

void mem::write(mem_info* data_, wb_info* wb_, cpu_info* status_, pc_info* pc_, dec_info* dec_) {

	if(busy)
		wait(free);

	busy = 1;

	if(data_) {
		mem_data = new mem_info;
		memcpy(mem_data, data_, sizeof(mem_info));
	}

	if(wb_) {
		wb_data = new wb_info;
		memcpy(wb_data, wb_, sizeof(wb_info));
	}

	memcpy(pc_data, pc_, sizeof(pc_info));
	memcpy(dec_data, dec_, sizeof(dec_info));
	memcpy(cpu_status, status_, sizeof(cpu_info));

	operate.notify();
}

/*
 * 	process
 */

void mem::do_run() {


	while(1) {

		busy = 0;
		free.notify();

#ifdef cpu_debug2
		std::cout << "DEBUG - mem::do_run() - aguardando para novo acesso a memoria\n";
#endif

		wait(operate);

#ifdef cpu_debug1
		std::cout << "DEBUG - mem::do_run() - executando acesso a memoria\n";
#endif


		if((cpu_status->skip == 0) && (mem_data != NULL))
			do_mem_access();

		wb->write(wb_data, cpu_status, pc_data, dec_data);

		if(mem_data) { delete mem_data; mem_data = NULL; }
		if(wb_data) { delete wb_data; wb_data = NULL; }
	}
}

/*
 *  internal functions
 */

void mem::do_mem_access() {

	if(mem_data->mem_r_w == 1)
		do_read();
	else
		do_write();
}

void mem::do_write() {

	mem_ctrl->lock();
	//if(mem_data->address == UART_ADDR_START)
	//	std::cout << "WB: " << std::setfill('0') << std::setw(8) << std::hex << mem_data->data << "\n";
	mem_ctrl->write(mem_data->data, mem_data->address, mem_data->size);
}

void mem::do_read() {

	unsigned int data;

	mem_ctrl->lock();
	mem_ctrl->read(&data, mem_data->address, mem_data->size);

	wb_data = new wb_info;

	switch(mem_data->size) {
	case 4:
		wb_data->result = data;
		break;
	case 2:
		if(mem_data->un_sig == 1)
			wb_data->result = (unsigned short)data;
		else
			wb_data->result = (signed short)data;
		break;
	case 1:
		if(mem_data->un_sig == 1)
			wb_data->result = (unsigned char)data;
		else
			wb_data->result = (signed char)data;
		break;
	}
	wb_data->reg_dest = mem_data->reg_dest;
	wb_data->un_sig = mem_data->un_sig;
}
