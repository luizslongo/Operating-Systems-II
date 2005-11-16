// EPOS-- ATMega128_Sensor Initialization

#include <mach/atmega128/sensor.h>

__BEGIN_SYS

// Class initialization
int ATMega128_Temperature_Sensor::init(System_Info * si)
{
    db<ATMega128_Temperature_Sensor>(TRC) << "ATMega128_Temperature_Sensor::init()\n";
    
    return 0;
}

int ATMega128_Light_Sensor::init(System_Info * si)
{
    db<ATMega128_Light_Sensor>(TRC) << "ATMega128_Light_Sensor::init()\n";
    
    return 0;
}

__END_SYS
