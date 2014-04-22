#ifndef __panda_machine_h
#define __panda_machine_h

#include <machine.h>
#include <cpu.h>
#include <mmu.h>
#include <tsc.h>

__BEGIN_SYS

class PandaBoard: public Machine_Common
{
    static const unsigned long CM_CTRL = 0x1000000C;
public:
    PandaBoard() {}

    static unsigned int cpu_id() { 
        int id;
        ASM("mrc p15, 0, %0, c0, c0, 5"
            : "=r"(id)
            : : );
        return id & 0xF; //cpu ID varies from 0 to 3
    }
    static unsigned int n_cpus() { return 1; }

    static void panic() { 
		db<PandaBoard>(ERR) << "PANIC!\n";
		CPU::int_disable();
		for(;;);
	}
    static void reboot() { 
        db<PandaBoard>(TRC) << "Machine::reboot()\n";
        // qemu is messing with the console when we reset the board
        // this for() will avoid reseting by now
        //for(;;);
        CPU::out32(CM_CTRL, CPU::in32(CM_CTRL) | (1 << 3)); 
    }
    static void poweroff() {  for(;;); }

    static void init(); 

    static void smp_barrier() {};
    static void smp_init(unsigned int) {};
private:

};
class Zynq: public PandaBoard
{
	public:
	Zynq(){}
};

__END_SYS

#include "flash.h"
#include "info.h"
#include "uart.h"
#include "rtc.h"
#include "timer.h"
#include "adc.h"
#include "nic.h"
#include "scu.h"

#endif
