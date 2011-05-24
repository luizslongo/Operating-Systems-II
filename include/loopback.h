// EPOS Loopback network interface

#ifndef __loopback_h
#define __loopback_h

#include <nic.h>
#include <mutex.h>

__BEGIN_SYS

class Loopback : public NIC_Common {

    static const unsigned int MTU = 1024;

public:
    typedef NIC_Common::Address<1> Address;
    typedef NIC_Common::Statistics Statistics;
    typedef NIC_Common::Protocol   Protocol;
    typedef char                   PDU[MTU];

    enum {
        ELP,
        IP,
        ARP,
        RARP
    };

    Loopback(unsigned int unit=0)
    : _unit(unit), _size(0) {}

    int send(const Address& dst,const Protocol& prot,
             const char * data,unsigned int size);

    int receive(Address * src,Protocol * prot,
                char * data,unsigned int size);

    void reset();

    unsigned int mtu() const { return MTU; }

    const Address & address() { return _address; }

    const Statistics & statistics() { return _stats; }

    static void init() {}

protected:
    unsigned int _unit;
    Address      _address;
    Statistics   _stats;
    PDU          _data;
    unsigned int _size;
    Protocol     _prot;
    Mutex        _lock;
};

__END_SYS

#endif
