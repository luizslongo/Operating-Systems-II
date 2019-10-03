// EPOS Raspberry Pi3 (ARM Cortex-A53) Memory Map

#ifndef __raspberry_pi3_memory_map_h
#define __raspberry_pi3_memory_map_h

#include <system/memory_map.h>
#include <machine/cortex/memory_map.h>

__BEGIN_SYS

struct Memory_Map: public Cortex_Memory_Map
{
    // Physical Memory
    enum {
        UART0_BASE              = 0x3f200000, // PrimeCell PL011 UART
        TIMER0_BASE             = 0x3f003000,
        AUX_BASE                = 0x3f215000, // mini UART + 2 x SPI master
    };

    // Logical Address Space
};

__END_SYS

#endif
