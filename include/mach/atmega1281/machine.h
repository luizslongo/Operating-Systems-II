// EPOS-- ATMega1281 Mediator Declarations

#ifndef __atmega1281_h
#define __atmega1281_h

#include <cpu.h>
#include <mmu.h>
#include <tsc.h>
#include "../avr_common/machine.h"
#include "memory_map.h"

__BEGIN_SYS

class ATMega1281: public Machine_Common, private AVR_Machine
{
public:
    typedef IO_Map<ATMega1281> IO;

public:
    template<typename Dev>
    static Dev * seize(const Type_Id & type, unsigned int unit) {
        //not implemented
	return 0;
    }

    static void release(const Type_Id & type, unsigned int unit) {
        //not implemented
    }

    static void panic() { AVR_Machine::panic(); }
    static void reboot() { CPU::reboot(); }
    static void poweroff();

    static int irq2int(int i) { return i; }
    static int int2irq(int i) { return i; }

    static unsigned int n_cpus() { return 0; }
    static unsigned int cpu_id() { return 0; }
    static void smp_init(unsigned int n_cpus) { }
    static void smp_barrier(int n_cpus = 0) { }

    static void init();
};

__END_SYS

#include "ic.h"
#include "timer.h"
#include "rtc.h"
#include "eeprom.h"
#include "flash.h"
#include "uart.h"
#include "spi.h"
#include "nic.h"
#include "adc.h"
#include "sensor.h"
#include "transceiver.h"

#endif

