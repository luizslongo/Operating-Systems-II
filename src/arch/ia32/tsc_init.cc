// EPOS-- IA32_TSC Initialization

#include <tsc.h>

__BEGIN_SYS

int IA32_TSC::init(System_Info * si)
{
    db<IA32_TSC>(TRC) << "IA32_TSC::init()\n";

    return 0;
}

__END_SYS
