// EPOS-- Sensor Mediator Common Package

#ifndef __sensor_h
#define __sensor_h

#include <system/config.h>

__BEGIN_SYS

class Sensor_Common
{
protected:
    Sensor_Common() {}
};

class Temperature_Sensor_Common: public Sensor_Common
{
protected:
    Temperature_Sensor_Common() {}
};

class Light_Sensor_Common: public Sensor_Common
{
protected:
    Light_Sensor_Common() {}
};

__END_SYS

#ifdef __SENSOR_H
#include __SENSOR_H
#endif

#endif
