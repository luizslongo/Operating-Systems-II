// EPOS-- ATMega128 Sensor Mediator Declarations

#ifndef __atmega128_sensor_h
#define __atmega128_sensor_h

#include <sensor.h>
#include "../avr_common/adc.h"

__BEGIN_SYS

class ATMega128_Sensor
{
protected:
    typedef IO_Map<Machine> IO;
    typedef AVR8::Reg8 Reg8;

    static const unsigned long CLOCK = Traits<Machine>::CLOCK;

    enum {
	CHANNEL  = 1,
	PORT     = IO::PORTE,
	DDR      = IO::DDRE,
	LIGHT_EN = 0x20,
	TEMP_EN  = 0x40,
    };

protected:
    ATMega128_Sensor() : _adc(CHANNEL, CLOCK >> 7) {}

public:
    void enable() { _adc.enable();  }
    void disable() { _adc.disable(); }

    int get() { return _adc.get(); }

    bool data_ready() { return _adc.finished(); }

    Reg8 port(){ return AVR8::in8(PORT); }
    void port(Reg8 value){ AVR8::out8(PORT,value); }   
    Reg8 ddr(){ return AVR8::in8(DDR); }
    void ddr(Reg8 value){ AVR8::out8(DDR,value); }   

private:
    ADC _adc;
};

class ATMega128_Temperature_Sensor: public Temperature_Sensor_Common,
				 private ATMega128_Sensor
{
public:
    ATMega128_Temperature_Sensor() {}
    ~ATMega128_Temperature_Sensor() { disable(); }

    void enable() {
	ddr(ddr() | TEMP_EN & ~LIGHT_EN);
	port(port() | TEMP_EN & ~LIGHT_EN);
	ATMega128_Sensor::enable();
    }

    void disable() {
	port(port() & ~TEMP_EN);
	ddr(ddr() & ~TEMP_EN);
	ATMega128_Sensor::disable();
    }

    // This is a very rough approximation funcion, but it works fine
    // in the range of 0-50 Celsius Degrees.
    int get() { return ((ATMega128_Sensor::get() >> 2) - 51) / 3; }

    int get_raw() { return ATMega128_Sensor::get(); }
};

class ATMega128_Light_Sensor: public ATMega128_Sensor
{
public:
    ATMega128_Light_Sensor() {}
    ~ATMega128_Light_Sensor() { disable(); }

    void enable() {
	ddr(ddr() | LIGHT_EN & ~TEMP_EN);
	port(port() | LIGHT_EN & ~TEMP_EN);
	ATMega128_Sensor::enable();
    }

    void disable() {
	port(port() & ~LIGHT_EN);
	ddr(ddr() & ~LIGHT_EN);
	ATMega128_Sensor::disable();
    }

    int get() { return ATMega128_Sensor::get() / 10; }
};

__END_SYS

#endif
