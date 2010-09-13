// EPOS PLASMA Mediator Declarations

#ifndef __plasma_h
#define __plasma_h

#include <utility/list.h>
#include <arch/mips32/cpu.h>
#include <arch/mips32/mmu.h>
#include <arch/mips32/tsc.h>
#include <machine.h>
#include "ic.h"
#include "device.h"

__BEGIN_SYS


class PLASMA: public Machine_Common
{
private:
    static const unsigned int INT_VECTOR_ADDRESS = Traits<PLASMA>::INT_VECTOR_ADDRESS;
    static const unsigned int INT_VECTOR_SIZE = 8;

    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Context Context;
    typedef CPU::Reg16 Reg16;

public:
    typedef void (int_handler)(unsigned int);

public:
    PLASMA() {}

    static int_handler * int_vector(unsigned int i) {
	if (i < INT_VECTOR_SIZE) 
	    return _interrupt_vector[i];
        else {
            db<PLASMA>(WRN) << "interrupt number " << i << " is greater than VECTOR_SIZE " << INT_VECTOR_SIZE << "\n";
        return 0;
        }
    }

    static void int_vector(unsigned int i, int_handler * h) {
       if (i < INT_VECTOR_SIZE) _interrupt_vector[i] = h;
       if(i==3) _interrupt_vector[2] = h;
    }

    template<typename Dev>
    static Dev * seize(const Type_Id & type, unsigned int unit) { 
        return reinterpret_cast<Dev *>(PLASMA_Device::seize(type, unit));
    }

    static void release(const Type_Id & type, unsigned int unit) { 
        PLASMA_Device::release(type, unit); 
    }

    static int irq2int(int i) { return i; } 

    static int int2irq(int i) { return i; } 

    static void panic();

    static void reboot();

    static void poweroff();

    static void init();

private:
    static void int_no_handler(unsigned int interrupt);

    static void interrupt_dispatcher();

private:
    static int_handler * _interrupt_vector[INT_VECTOR_SIZE];
};

__END_SYS

#include "ic.h"
#include "timer.h"
#include "rtc.h"
#include "uart.h"
#include "display.h"
#include "nic.h"

#endif
