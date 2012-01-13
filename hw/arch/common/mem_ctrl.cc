/*
 * mem_ctrl.cpp
 *
 *  Created on: 29/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include <iomanip>
#include "mem_ctrl.h"

/*
 *  process
 */

void mem_ctrl::end_of_elaboration() {
	// perform a static check for overlapping memory areas of the slaves

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::end_of_elaboration()";
#endif

	bool no_overlap;
	for(int i = 1; i < module_port.size(); ++i) {
		module_if *module_1 = module_port[i];
		for(int j = 0; j < i; ++j) {
			module_if *module_2 = module_port[j];
			no_overlap = (module_1->get_end_address() < module_2->get_start_address()) ||
					(module_1->get_start_address() > module_2->get_end_address());
			if(!no_overlap) {
				std::cout << "Error: overlapping address spaces of 2 slaves :" << endl;
				std::cout << "module " << i << " 0x" << std::hex << module_1->get_start_address() << " 0x" << std::hex << module_1->get_end_address() << endl;
				std::cout << "module " << i << " 0x" << std::hex << module_2->get_start_address() << " 0x" << std::hex << module_2->get_end_address() << endl;
			}
		}
	}
}

module_if* mem_ctrl::get_module(unsigned int address) {

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::get_module(" << address.to_string(SC_HEX) << ")\n";
#endif

	for(int i = 0; i < module_port.size(); ++i) {
		module_if* module = module_port[i];
		if((module->get_start_address() <= address) &&
				(address <= module->get_end_address())) {

#ifdef mem_ctrl_debug
			std::cout << "mem_ctrl::get_module() - Module found\n";
#endif

			return module;
		}
	}

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::get_module() - Module not found\n";
#endif

	return (module_if*) 0;
}

/*
 *  interfaces
 */

void mem_ctrl::lock() {
	if(lock_)
		wait(free);

	lock_ = 1;
}

void mem_ctrl::read(unsigned int *data, unsigned int address, int size) {

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::read(*data, " << address.to_string(SC_HEX) << ", " << size << ")\n";
#endif

	if(address == 0xFFFFFFFC) {
		std::cout << "##INFO Exit command detected\n";
		sc_stop();
	}

	module_if	*module = get_module(address);
	if(!module) {
		std::cout << "mem_ctrl::read() - error: module no found";
		return;
	}

	module->read(data, address, size);

	switch(size) {
	case 4:
		assert((address &3) == 0);
		break;
	case 2:
		assert((address & 1) == 0);
		break;
	case 1:
		break;

	default: std::cout << "ERROR - READ MEMORY"; break;
	}

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::read():" << data->to_string(SC_HEX) << "\n";
#endif

	lock_ = 0;
	free.notify();
}

void mem_ctrl::write(unsigned int data, unsigned int address, int size) {

	unsigned int data_;

	data_ = 0;

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::write("<< data.to_string(SC_HEX) << ", " << address.to_string(SC_HEX) << ", " << size << ")\n";
#endif

	if(address == 0xFFFFFFFC) {
		std::cout << "##INFO Exit command detected\n";
		sc_stop();
	}

	module_if	*module = get_module(address);
	if(!module) {
		std::cout << "error mem_ctrl::write()";
		return;
	}

	switch(size) {
	case 4:
		assert((address & 3) == 0);
		data_ = data;
		break;
	case 2:
		assert((address & 1) == 0);
		data_ = (unsigned short) data;
		break;
	case 1:
		data_ = (unsigned char) data;
		break;
	default:
		std::cout << "ERROR - WRITE MEMORY";
	}

	module->write(data_, address, size);

	lock_ = 0;
	free.notify();

#ifdef mem_ctrl_debug
	std::cout << "mem_ctrl::write(): null\n";
#endif
}
