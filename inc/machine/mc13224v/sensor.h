// EPOS MC13224V Sensor Mediator Declarations

#ifndef __mc13224v_sensor_h
#define __mc13224v_sensor_h

#include <sensor.h>
#include "ertj1vg103fa.h"

__BEGIN_SYS

class MC13224V_Temperature_Sensor: public Temperature_Sensor_Common
{
private:
    typedef Traits<MC13224V_Temperature_Sensor>::SENSORS SENSORS;
    static const unsigned int UNITS = SENSORS::Length;

public:
    MC13224V_Temperature_Sensor() {
        _dev = new Meta_Temperature_Sensor<SENSORS>::Get<0>::Result;
    }
    template<unsigned int UNIT>
    MC13224V_Temperature_Sensor(unsigned int u) {
        _dev = new typename Meta_Temperature_Sensor<SENSORS>::Get<UNIT>::Result(UNIT);
    }

    ~MC13224V_Temperature_Sensor() {
        delete _dev;
    }

    int sample() {
        return _dev->sample();
    }

    int get() {
        return _dev->get();
    }

    bool enable() {
        return _dev->enable();
    }

    void disable() {
        _dev->disable();
    }

    bool data_ready() {
        return _dev->data_ready();
    }

    static void init();

private:
    Meta_Temperature_Sensor<SENSORS>::Base * _dev;
};

__END_SYS

#endif
