// EPOS-- PC_Sensor Declarations

#ifndef __dummy_sensor_h
#define __dummy_sensor_h

#include <sensor.h>

__BEGIN_SYS

class Dummy_Sensor: public Sensor_Common
{

public:

    Dummy_Sensor() {}

    void enable() { }
    void disable() { }

    void int_enale() { }
    void int_disable() { }

    int get() {return 0; }

    bool data_ready() { return false; }

    static int init(System_Info *si) { return 0; }

};

__END_SYS

#endif
