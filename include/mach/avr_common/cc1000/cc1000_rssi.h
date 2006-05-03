// EPOS-- CC1000 RSSI Sensor Mediator

#ifndef __cc1000_rssi_h
#define __cc1000_rssi_h

#include <adc.h>

__BEGIN_SYS

class CC1000_RSSI
{
protected:
    typedef AVR8::Reg8 Reg8;

    static const unsigned long CLOCK = Traits<Machine>::CLOCK;

    enum {
	CHANNEL  = 0,
    };

public:
    CC1000_RSSI() : _adc(CHANNEL, CLOCK >> 7) {}

public:
    bool enable() { return _adc.enable();  }
    void disable() { _adc.disable(); }

    bool data_ready() { return _adc.finished(); }

    int get() { return _adc.get(); }

    int sample() { return _adc.sample(); }


private:
    ADC _adc;
};

__END_SYS

#endif
