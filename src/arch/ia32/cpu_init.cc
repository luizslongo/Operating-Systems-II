// EPOS IA32 Initialization
//
// Author: Guto
// Documentation: $EPOS/doc/cpu			Date: 15 Jun 2003

#include <arch/ia32/cpu.h>

__BEGIN_SYS

// Class initialization
int IA32::init(System_Info * si)
{
    db<IA32>(TRC) << "IA32::init()\n";

    return 0;
}

__END_SYS
