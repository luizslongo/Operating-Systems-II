// EPOS-- Mica Sensor Board Common Declarations

#ifndef __micasb_sensor_h
#define __micasb_sensor_h

#include "analog_sensor.h"

__BEGIN_SYS

class MicaSB_Sensor: public Analog_Sensor
{
protected:

    typedef IO_Map<Machine> IO;
    typedef AVR8::Reg8 Reg8;

    enum {
	CHANNEL  = 1,
	PORT     = IO::PORTE,
	DDR      = IO::DDRE,
	LIGHT_EN = 0x20,
	TEMP_EN  = 0x40,
    };

public:
    
    MicaSB_Sensor() : Analog_Sensor(CHANNEL) { }

protected:

    Reg8 port(){ return AVR8::in8(PORT); }
    void port(Reg8 value){ AVR8::out8(PORT,value); }   
    Reg8 ddr(){ return AVR8::in8(DDR); }
    void ddr(Reg8 value){ AVR8::out8(DDR,value); }   
    
};

class MicaSB_Temperature_Sensor: public MicaSB_Sensor
{

public:

    ~MicaSB_Temperature_Sensor () { disable(); }

    void enable() {
	ddr(ddr() | TEMP_EN & ~LIGHT_EN);
	port(port() | TEMP_EN & ~LIGHT_EN);
	Analog_Sensor::enable();
    }

    void disable() {
	port(port() & ~TEMP_EN);
	ddr(ddr() & ~TEMP_EN);
	Analog_Sensor::disable();
    }

    // This is a very rough approximation funcion, but it works fine
    // in the range of 0-50 Celsius Degrees.
    int get() {
	return ((MicaSB_Sensor::get()>>2) - 51) / 3;
    }

    int get_raw() {
	return MicaSB_Sensor::get();
    }

};

class MicaSB_Light_Sensor: public MicaSB_Sensor
{

public:

    ~MicaSB_Light_Sensor() { disable(); }

    void enable() {
	ddr(ddr() | LIGHT_EN & ~TEMP_EN);
	port(port() | LIGHT_EN & ~TEMP_EN);
	Analog_Sensor::enable();
    }

    void disable() {
	port(port() & ~LIGHT_EN);
	ddr(ddr() & ~LIGHT_EN);
	Analog_Sensor::disable();
    }

    int get() {
	return MicaSB_Sensor::get() / 10;
    }

};

__END_SYS

#endif

