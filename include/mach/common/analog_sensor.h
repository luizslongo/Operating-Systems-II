// EPOS-- Analog_Sensor Declarations

#ifndef __analog_sensor_h
#define __analog_sensor_h

#include <sensor.h>
#include <adc.h>

__BEGIN_SYS

class Analog_Sensor: public Sensor_Common
{

public:

    Analog_Sensor(unsigned char channel) 
	: _adc(channel,ADC::SYSTEM_REF,	
	       ADC::FREE_RUNNING_MODE,ADC::CLOCK >> 7) { }

    void enable() { _adc.enable();  }
    void disable() { _adc.disable(); }

    void int_enale() { _adc.int_enable();  }
    void int_disable() { _adc.int_disable(); }

    int get() { return _adc.get(); }

    bool data_ready() { return _adc.adc_complete(); }

protected:

    ADC _adc;

};

__END_SYS

#endif

