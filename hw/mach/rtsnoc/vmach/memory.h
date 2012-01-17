/*
 * cache.h
 *
 *  Created on: 03/03/2010
 *      Author: tiago
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <systemc.h>
#include <arch/common/module_if.h>

class memory : public sc_module,
	public module_if {

public:
	memory(sc_module_name nm_
			, unsigned int start_address_
			, unsigned int end_address_
			, unsigned int size_)
		: sc_module(nm_)
		, start_address(start_address_)
		, end_address(end_address_)
		, mem_size(size_)
	{
		m = new unsigned char [size_];

		memset(m, 0, sizeof(char) * size_);
	}

	~memory() {
		if(m) {
			delete [] m;
			m = (unsigned char *)0;
		}
	}
	//interfaces
	void read(unsigned int *data, unsigned int address, int size);
	void write(unsigned int data, unsigned int address, int size);

	unsigned int get_start_address() const;
	unsigned int get_end_address() const;

private:

	unsigned char 	*m;

	unsigned int	start_address;
	unsigned int	end_address;
	unsigned int 	mem_size;
};

#endif /* MEMORY_H_ */
