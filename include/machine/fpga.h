// EPOS FPGA Mediator Common Package

#ifndef __fpga_h
#define __fpga_h

#include <architecture/cpu.h>

__BEGIN_SYS

class FPGA_Common
{
protected:
    FPGA_Common() {}
};

__END_SYS

#endif

#if defined(__FPGA_H) && !defined(__common_only__)
#include __FPGA_H
#endif
