#ifndef __panda_nic_h
#define __panda_nic_h

#include <ethernet.h>

__BEGIN_SYS

class PandaBoard_NIC: public Ethernet
{
public:
    
    PandaBoard_NIC(unsigned int u = 0) {
    }

    ~PandaBoard_NIC() { }
    
    int send(const Address & dst, const Protocol & prot, 
             const void * data, unsigned int size) {
        return 0; 
    }

    int receive(Address * src, Protocol * prot,
                void * data, unsigned int size) {
        return 0; 
    }

    void reset() { }

    unsigned int mtu() const { return 0; }
    
    const Address & address() { return _address; }

    const Statistics & statistics() { return _statistics; }

    static void init();

private:
    Address _address;
    Statistics _statistics;
};

__END_SYS

#endif
