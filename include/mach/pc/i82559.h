// EPOS-- PC Ethernet NIC Mediator

#ifndef __pc_ethernet_h
#define __pc_ethernet_h

#include <nic.h>

__BEGIN_SYS

class i82559: public Ethernet_NIC_Common
{
public:
    i82559(unsigned int unit = 0);
    ~i82559();

    int send(const Address & to, const Protocol & prot,
	     const void * data, unsigned int size);
    int receive(Address * from, Protocol * prot,
		void * data, unsigned int size);
};

__END_SYS

#endif
