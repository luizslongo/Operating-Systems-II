// EPOS-- PC Mediator Declarations

#ifndef __pc_h
#define __pc_h

#include <utility/list.h>
#include <arch/ia32/cpu.h>
#include <arch/ia32/mmu.h>
#include <arch/ia32/tsc.h>
#include <machine.h>
#include "device.h"

__BEGIN_SYS

class PC: public Machine_Common
{
private:
    typedef Traits<PC> Traits;
    static const Type_Id TYPE = Type<PC>::TYPE;

    static const unsigned int HARD_INT = Traits::HARDWARE_INT_OFFSET;
    static const unsigned int SYSCALL_INT = Traits::SYSCALL_INT;
    static const unsigned int INT_VECTOR_SIZE = 64;
    
    typedef IA32::Reg32 Reg32;
    typedef IA32::Log_Addr Log_Addr;

public:
    typedef void (int_handler)(unsigned int);

public:
    PC() {}
  
    static int_handler * int_vector(unsigned int i) {
	return (i < INT_VECTOR_SIZE) ? _int_vector[i] : 0;
    }
    static void int_vector(unsigned int i, int_handler * h) {
	db<PC>(INF) << "PC::int_vector(int=" << i << ",h=" << h <<")\n";
	if(i < INT_VECTOR_SIZE) _int_vector[i] = h;
    }

    template<typename Dev>
    static Dev * seize(const Type_Id & type, unsigned int unit) { 
	return reinterpret_cast<Dev *>(PC_Device::seize(type, unit));
    }

    static void release(const Type_Id & type, unsigned int unit) { 
	PC_Device::release(type, unit); 
    }

    static void panic();
    static void reboot();
    static void poweroff();

    // PC specific methods
    static int irq2int(int i) { return i + HARD_INT; }
    static int int2irq(int i) { return i - HARD_INT; }
    
    static int init(System_Info * si);

private:
    static void int_dispatch();

    static void int_not(unsigned int interrupt);
    static void exc_not(unsigned int interrupt,
			Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags);
    static void exc_pf (unsigned int interrupt,
			Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags);
    static void exc_gpf(unsigned int interrupt,
			Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags);
    static void exc_fpu(unsigned int interrupt,
			Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags);

private:
    static int_handler * _int_vector[INT_VECTOR_SIZE];
};

typedef PC Machine;

__END_SYS

#include "ic.h"
#include "pci.h"
#include "timer.h"
#include "rtc.h"
#include "uart.h"
#include "display.h"
#include "sensor.h"
#include "nic.h"

#endif
