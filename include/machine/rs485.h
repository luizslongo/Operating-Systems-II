// EPOS RS485 Mediator Common Package

#ifndef __RS485_h
#define __RS485_h

#include <system/config.h>

__BEGIN_SYS

class RS485_Common
{
protected:
    RS485_Common() {}
};

__END_SYS

#endif

#if defined(__RS485_H) && !defined(__common_only__)
#include __RS485_H
#endif
