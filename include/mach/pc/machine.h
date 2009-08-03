// EPOS-- PC Mediator Declarations

#ifndef __pc_h
#define __pc_h

#include <utility/list.h>
#include <cpu.h>
#include <mmu.h>
#include <tsc.h>
#include <machine.h>
#include "apic.h"

__BEGIN_SYS

class PC: public Machine_Common
{
private:
    static const unsigned int HARD_INT = Traits<PC>::HARDWARE_INT_OFFSET;
    static const unsigned int SYSCALL_INT = Traits<PC>::SYSCALL_INT;
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
	db<PC>(INF) << "PC::int_vector(int=" << i << ",h=" 
		    << (void *)h <<")\n";
	if(i < INT_VECTOR_SIZE) _int_vector[i] = h;
    }

    static void panic();
    static void reboot();
    static void poweroff();

    static int irq2int(int i) { return i + HARD_INT; }
    static int int2irq(int i) { return i - HARD_INT; }
    
    static unsigned int n_cpus() { return _n_cpus; }
    static unsigned int cpu_id() { return APIC::id(); }

    static void smp_init(unsigned int n_cpus) {
	_n_cpus = n_cpus;
	APIC::remap(); 
    };

    static void smp_barrier(unsigned int n_cpus = _n_cpus) {
	_bp_finished = false;
	if(cpu_id() == 0) { // Boot strap CPU (BSP)
 	    db<Machine>(TRC) << "CPU=" << cpu_id() << "/" << n_cpus << "\n";
	    // Wait for other CPUs to finish SETUP
	    while(_ap_finished < (n_cpus - 1));

	    // Signalize other CPUs they can preceed to the next stage
	    _bp_finished = true;
	    _ap_finished = 0;
	} else { // Additional CPUs (APs)
 	    db<Machine>(TRC) << "CPU=" << cpu_id() << "/" << n_cpus << "\n";
	    // Signalize the Boot CPU that this CPU is finished with SETUP
	    CPU::finc(reinterpret_cast<volatile int &>(_ap_finished));
    
	    while(!_bp_finished);
	}
    }

    static void init();

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
    static volatile unsigned int  _n_cpus;
    static volatile bool _bp_finished;
    static volatile unsigned int  _ap_finished;
    static int_handler * _int_vector[INT_VECTOR_SIZE];
};

__END_SYS

#include "info.h"
#include "memory_map.h"
#include "ic.h"
#include "pci.h"
#include "timer.h"
#include "rtc.h"
#include "eeprom.h"
#include "uart.h"
#include "display.h"
#include "nic.h"

#endif
