// EPOS-- Mica2_Sensor Initialization

#include <mach/mica2/sensor.h>

__BEGIN_SYS

// Class initialization
int Mica2_Temperature_Sensor::init(System_Info * si)
{
    db<Mica2_Temperature_Sensor>(TRC) << "Mica2_Temperature_Sensor::init()\n";
    
    return 0;
}

int Mica2_Light_Sensor::init(System_Info * si)
{
    db<Mica2_Light_Sensor>(TRC) << "Mica2_Light_Sensor::init()\n";
    
    return 0;
}

__END_SYS
