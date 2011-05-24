// EPOS Serial line IP interface

#ifndef __slip_h
#define __slip_h

#include <nic.h>
#include <mutex.h>

__BEGIN_SYS

// See: http://www.faqs.org/rfcs/rfc1055.html

class SLIP : public NIC_Common {
public:
    static const unsigned int MTU = 1006;

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

    //* Open SLIP interface on UART 'unit'
    SLIP(unsigned int unit=0)
    : _unit(unit), _uart(unit) {}

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

    static const char END = 0xc0;
    static const char ESC = 0xdb;
    static const char ESC_END = 0xdc;
    static const char ESC_ESC = 0xdd;


    unsigned int _unit;
    Address      _address;
    Statistics   _stats;
    Mutex        _tx_lock;
    Mutex        _rx_lock;
    UART         _uart;
};

__END_SYS

#endif
