// EPOS-- Machine Mediator Common Package

#ifndef __machine_h
#define __machine_h

#include <system/config.h>

__BEGIN_SYS

class Machine_Common
{
protected:
    Machine_Common() {}

public:
    typedef void (* Handler)(void);
};

__END_SYS

#include __HEADER_MACH(MACH)

#endif
