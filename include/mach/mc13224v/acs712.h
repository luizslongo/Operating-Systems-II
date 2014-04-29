// EPOS ACS712 Sensor Mediator Declarations

#ifndef __acs712_h
#define __acs712_h

#include <adc.h>

__BEGIN_SYS

class ACS712 {
public:
    ACS712(int adc_channel = ADC::SINGLE_ENDED_ADC0, int adc_unit = 0,
           int bias = 2926, int frequency = Traits<Machine>::CLOCK>>9)
    : _bias(bias),
      _adc(adc_channel, ADC::SYSTEM_REF, ADC::SINGLE_CONVERSION_MODE,
           frequency, adc_unit) {}

    ~ACS712() {}

    int get() { return (_adc.get() - _bias); }
    int sample() { return (_adc.get() - _bias); }
    bool enable() { return _adc.enable();}
    void disable() { _adc.disable(); }
    bool data_ready() { return _adc.finished(); }

private:
    int _bias;
    ADC _adc;
};

__END_SYS

#endif
