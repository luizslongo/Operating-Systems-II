// EPOS-- ATMega128_Sensor Declarations

#ifndef __atmega128_sensor_h
#define __atmega128_sensor_h

#include <sensor.h>
#include "../common/micasb_sensor.h"

__BEGIN_SYS

class ATMega128_Temperature_Sensor: public MicaSB_Temperature_Sensor
{
private:
    typedef Traits<ATMega128_Temperature_Sensor> Traits;
    static const Type_Id TYPE = Type<ATMega128_Temperature_Sensor>::TYPE;

public:

    static int init(System_Info *si);
    
};

class ATMega128_Light_Sensor: public MicaSB_Light_Sensor
{
private:
    typedef Traits<ATMega128_Light_Sensor> Traits;
    static const Type_Id TYPE = Type<ATMega128_Light_Sensor>::TYPE;

public:

    static int init(System_Info *si);
    
};

typedef ATMega128_Temperature_Sensor Temperature_Sensor;
typedef ATMega128_Light_Sensor Light_Sensor;

__END_SYS

#endif

