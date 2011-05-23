// EPOS ATMega16 NIC Mediator Declarations

#ifndef __at90can128_nic_h
#define __at90can128_nic_h

#include <radio.h>

__BEGIN_SYS

class AT90CAN128_NIC: public Radio_Common
{

public:
    AT90CAN128_NIC() {    }
    AT90CAN128_NIC(unsigned int u) {    }
    ~AT90CAN128_NIC() {    }

    int send(const Address & dst, const Protocol & prot, 
         const void * data, unsigned int size) {
    return 0;
    }
    int receive(Address * src, Protocol * prot,
        void * data, unsigned int size) {
    return 0;
    }

    void reset() { }

    unsigned int mtu() const {return 0; }

    const Address & address() { return _addr; }

    const Statistics & statistics() { return _stats; }

    static void init();

 private:

    Address _addr;
    Statistics _stats;

};

__END_SYS

#endif
