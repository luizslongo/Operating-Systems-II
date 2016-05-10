// EPOS TSTP over Ethernet Protocol Declarations

#include <nic.h>
#include <tstp.h>
#include <rtc.h>

#ifndef __tstpoe_h
#define __tstpoe_h

__BEGIN_SYS

class TSTPOE: private TSTP_Common, private NIC::Observer
{
    friend class System;
    template<int unit> friend void call_init();

public:
    static const unsigned int MTU = 1514;

    // Buffers received from the NIC
    typedef NIC::Buffer Buffer;

    // TSTPOE observer/d is not conditinal: all packets are passed to TSTP
    typedef Data_Observer<Buffer> Observer;
    typedef Data_Observed<Buffer> Observed;

protected:
    template<unsigned int UNIT = 0>
    TSTPOE(unsigned int nic = UNIT): _nic(nic) {
        db<TSTPOE>(TRC) << "TSTPOE::TSTPOE(nic=" << &_nic << ") => " << this << endl;

        _nic.attach(this, NIC::TSTP);
    }

public:
    ~TSTPOE() {
        db<TSTPOE>(TRC) << "TSTPOE::~TSTPOE(nic=" << &_nic << ") => " << this << endl;

        _nic.detach(this, NIC::TSTP);
    }

    static Buffer * alloc(unsigned int payload)
    {
        db<TSTPOE>(TRC) << "TSTPOE::alloc(pl=" << payload << ")" << endl;

        return nic()->alloc(nic(), NIC::Address::BROADCAST, NIC::TSTP, 0, 0, payload);
    }

    static int send(Buffer * buf) {
        db<TSTPOE>(TRC) << "TSTPOE::send(buf=" << buf << ")" << endl;

        return nic()->send(buf); // implicitly releases the buffer
    }

    static Coordinates here() { return (nic()->address())[5] % 2 ? Coordinates(0, 0, 0) : Coordinates(10, 10, 10); }
    static Time now() { return RTC::seconds_since_epoch(); }

    static NIC * nic() { return &(_networks[0]->_nic); }
    static const unsigned int mtu() { return MTU; }

    static void attach(Observer * obs) { _observed.attach(obs); }
    static void detach(Observer * obs) { _observed.detach(obs); }
    static bool notify(Buffer * buf) { return _observed.notify(buf); }

private:
    void update(NIC::Observed * obs, NIC::Protocol prot, Buffer * buf) {
        db<TSTPOE>(TRC) << "TSTPOE::update(obs=" << obs << ",prot=" << hex << prot << dec << ",buf=" << buf << ")" << endl;
        buf->nic(&_nic);
        if(!notify(buf))
            _nic.free(buf);
    }

    static void init(unsigned int unit) {
         db<Init, TSTP>(TRC) << "TSTPOE::init(u=" << unit << ")" << endl;

         _networks[unit] = new (SYSTEM) TSTPOE(unit);
     }

protected:
    NIC _nic;

    static TSTPOE * _networks[Traits<NIC>::UNITS];
    static Observed _observed; // shared by all TSTPOE instances, so the default for binding on a unit is for all NICs
};

typedef TSTPOE TSTPNIC;

__END_SYS

#endif
