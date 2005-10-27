// EPOS-- IC Mediator Common Package

#ifndef __ic_h
#define __ic_h

#include <system/config.h>

__BEGIN_SYS

class IC_Common
{
protected:
    IC_Common() {};

public:
    // Hardware interrupt number
    typedef int IRQ;
    enum {ALL = -1};
};

__END_SYS

#include __HEADER_MACH(ic)

#endif
