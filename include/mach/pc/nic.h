// EPOS-- PC NIC Mediator Declarations

#ifndef __pc_nic_h
#define __pc_nic_h

#include "machine.h"
#include "pcnet32.h"
#include "e100.h"
#include "c905.h"

__BEGIN_SYS

class PC_NIC: public NIC_Common
{
private:
    typedef Traits<PC_NIC>::NICS NICS;
    static const unsigned int UNITS = NICS::Length;

public:
    PC_NIC() {
	_dev = new Meta_NIC<NICS>::Get<0>::Result;
    }
    template<unsigned int UNIT>
    PC_NIC(unsigned int u) {
	_dev = new typename Meta_NIC<NICS>::Get<UNIT>::Result(UNIT);
    }
    ~PC_NIC() {
	delete _dev;
    }
    
    int send(const Address & dst, const Protocol & prot, 
	     const void * data, unsigned int size) {
	return _dev->send(dst, prot, data, size); 
    }
    int receive(Address * src, Protocol * prot,
		void * data, unsigned int size) {
	return _dev->receive(src, prot, data, size); 
    }
    
    void reset() { _dev->reset(); }

    unsigned int mtu() const { return _dev->mtu(); }
    
    const Address & address() { return _dev->address(); }

    const Statistics & statistics() { return _dev->statistics(); }

    static void init();

private:
    Meta_NIC<NICS>::Base * _dev;
};

 __END_SYS

#endif
