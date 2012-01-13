/*
 * cache.cpp
 *
 *  Created on: 03/03/2010
 *      Author: tiago
 */

#include <string>
#include <iostream>
#include <iomanip>
#include <plasma_pack.h>
#include "memory.h"

void memory::read(unsigned int *data, unsigned int address, int size) {
	unsigned long int ptr;

#ifdef memory_debug
	std::cout << "memory::read() - address: " << std::setfill('0') << std::setw(8) << std::hex << address << "\n";
#endif

	ptr = (unsigned long int) m + (address % mem_size);

	switch(size) {
	case 4:
		assert((address &3) == 0);
		*data = (unsigned int)ntohl(*(unsigned int*)ptr);
		break;
	case 2:
		assert((address & 1) == 0);
		*data = (unsigned short)ntohs(*(unsigned int*)ptr);
		break;
	case 1:
		*data = *(unsigned char*) ptr;
		break;

	default: std::cout << "ERROR - READ MEMORY"; break;
	}

#ifdef memory_debug
	std::cout << "memory::read() - valor lido: " << std::setfill('0') << std::setw(8) << std::hex << data_ << "\n";
#endif
}

void memory::write(unsigned int data, unsigned int address, int size) {
	unsigned long int ptr;

	ptr = (unsigned long int) m + (address % mem_size);

	switch(size) {
	case 4:
		//assert((address & 3) == 0);
		*(unsigned int*) ptr = (unsigned int)htonl(data);
		break;
	case 2:
		//assert((address & 1) == 0);
		*(unsigned short*) ptr = (unsigned short)htons(data);
		break;
	case 1:
		*(unsigned char*) ptr = (unsigned char)data;
		break;
	default:
		std::cout << "ERROR - WRITE MEMORY";
	}
}

unsigned int memory::get_start_address() const{
	return start_address;
}

unsigned int memory::get_end_address() const{
	return end_address;
}
