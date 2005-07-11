// EPOS-- Sentient Abstraction Initialization

#include <sentient.h>

__BEGIN_SYS

int Photo_Sentient::init(System_Info * si)
{
    db<Photo_Sentient>(TRC) << "Photo_Sentient::init()\n";

    return 0;
}

int Temperature_Sentient::init(System_Info * si)
{
    db<Temperature_Sentient>(TRC) << "Temperature_Sentient::init()\n";

    return 0;
}

__END_SYS
