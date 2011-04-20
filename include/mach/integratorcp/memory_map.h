// EPOS-- IntegratorCP Memory Map

#ifndef __memory_map_integratorcp_h
#define __memory_map_integratorcp_h

#include <system/memory_map.h>

// This must be redoned! Everything here was ripped from mc12234v
__BEGIN_SYS

template <>
struct Memory_Map<IntegratorCP>
{
    enum {
        MEM_BASE =  0,
        MEM_SIZE =  96 * 1024
    };

    enum {
        BASE =      0x00400000,
        TOP =       0x00418000,
        APP_LO =    0x00000000,
        APP_CODE =  0x00000000,
        APP_DATA =  0x00000000,
        APP_HI =    0x00000000,
        PHY_MEM =   0x00400000,
        IO_MEM =    0x80000000,
        APIC_MEM =  0x00000000,
        SYS =       0x00000000,
        IDT =       SYS + 0x00000000,
        GDT =       SYS + 0x00000000,
        SYS_PT =    SYS + 0x00000000,
        SYS_PD =    SYS + 0x00000000,
        SYS_INFO =  0x00416000,
        SYS_CODE =  SYS + 0x00000000,
        SYS_DATA =  SYS + 0x00000000,
        SYS_STACK = SYS + 0x00000000
    };
};

template <class Imp> struct IO_Map;
template <> struct IO_Map<IntegratorCP>
{
    typedef volatile unsigned char IO_Port;  
    struct IO_Ports {
    };
};

__END_SYS

#endif

