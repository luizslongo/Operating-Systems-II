// EPOS-- Mica2_Sensor Declarations

#ifndef __mica2_sensor_h
#define __mica2_sensor_h

#include <sensor.h>
#include "../common/micasb_sensor.h"

__BEGIN_SYS

class Mica2_Temperature_Sensor: public MicaSB_Temperature_Sensor
{
private:
    typedef Traits<Mica2_Temperature_Sensor> Traits;
    static const Type_Id TYPE = Type<Mica2_Temperature_Sensor>::TYPE;

public:

    static int init(System_Info *si);
    
};

class Mica2_Light_Sensor: public MicaSB_Light_Sensor
{
private:
    typedef Traits<Mica2_Light_Sensor> Traits;
    static const Type_Id TYPE = Type<Mica2_Light_Sensor>::TYPE;

public:

    static int init(System_Info *si);
    
};

typedef Mica2_Temperature_Sensor Temperature_Sensor;
typedef Mica2_Light_Sensor Light_Sensor;

__END_SYS

#endif

