// EPOS-- Sensor Mediator Common Package

#ifndef __sentient_h
#define __sentient_h

#include <system/config.h>
#include <sensor.h>

__BEGIN_SYS

class Sentient_Common
{

public:


};

class Temperature_Sentient : public Sentient_Common {

public:

    Temperature_Sentient() {
	_sensor.enable();
    }

    int get() { return _sensor.get(); }

    static int init(System_Info * si);

private:

    Temperature_Sensor _sensor;

};


class Photo_Sentient : public Sentient_Common  {

public:

    Photo_Sentient()  {
	_sensor.enable();
    }

    int get() { return _sensor.get();  }

    static int init(System_Info * si);

private:

    Light_Sensor _sensor;

};




__END_SYS

#endif
