// EPOS-- PC Mediator Declarations

#ifndef __pc_h
#define __pc_h

#include <utility/list.h>
#include <cpu.h>
#include <mmu.h>
#include <tsc.h>
#include <machine.h>
#include "info.h"
#include "memory_map.h"
#include "ic.h"

__BEGIN_SYS

class PC: public Machine_Common
{
private:
    static const bool smp = Traits<Thread>::smp;
    static const unsigned int SYSCALL_INT = Traits<PC>::SYSCALL_INT;
    
    typedef IA32::Reg32 Reg32;
    typedef IA32::Log_Addr Log_Addr;

public:
    PC() {}
  
    static void panic();
    static void reboot();
    static void poweroff();

    static unsigned int n_cpus() { return smp ? _n_cpus : 0; }
    static unsigned int cpu_id() { return smp ? APIC::id() : 0; }

    static void smp_init(unsigned int n_cpus) {
	if(smp) {
	    _n_cpus = n_cpus;
	    APIC::remap(); 
	}
    };

    static void smp_barrier(unsigned int n_cpus = _n_cpus) {
	if(smp) {
	    _bp_finished = false;
	    if(cpu_id() == 0) { // Boot strap CPU (BSP)
		db<Machine>(TRC) << "CPU=" << cpu_id() 
				 << "/" << n_cpus << "\n";
		// Wait for other CPUs to finish SETUP
		while(_ap_finished < (n_cpus - 1));

		// Signalize other CPUs they can preceed to the next stage
		_bp_finished = true;
		_ap_finished = 0;
	    } else { // Additional CPUs (APs)
		db<Machine>(TRC) << "CPU=" << cpu_id()
				 << "/" << n_cpus << "\n";
		// Signalize the Boot CPU that this CPU is finished with SETUP
		CPU::finc(reinterpret_cast<volatile int &>(_ap_finished));
    
		while(!_bp_finished);
	    }
	}
    }

    static void init();

private:
    static volatile unsigned int  _n_cpus;
    static volatile bool _bp_finished;
    static volatile unsigned int  _ap_finished;
};

__END_SYS

#include "pci.h"
#include "timer.h"
#include "rtc.h"
#include "eeprom.h"
#include "uart.h"
#include "display.h"
#include "nic.h"

#endif
