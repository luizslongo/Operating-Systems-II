// EPOS--  Memory Map for the ML310

#ifndef __memory_map_ml310_h
#define __memory_map_ml310_h

__BEGIN_SYS

template <>
struct Memory_Map<ML310>
{
    enum Memory {
        MEM_BASE =              0,
        MEM_SIZE =              64 * 1024 * 1024,
    };

    enum Map {
        BASE =          0x00000000,
        TOP =           0xFFFFFFFF,
        APP_LO =        0x00000000,
        APP_CODE =      0x00000000,
        APP_DATA =      0x00200000,
        APP_HI =        0x03800000,
        PHY_MEM =       0x04000000,
        IO_MEM =        0x00000000,
        SYS =           0x03800000,
        INT_VEC =       0x03FF0000,
        SYS_PT =        0x00000000,
        SYS_PD =        0x00000000,
        SYS_INFO =      0x03FF4000,
        SYS_CODE =      0x03800000,
        SYS_DATA =      0x03A00000,
        SYS_STACK =     0x03FEFFF8,
        MACH1 =         TOP,
        MACH2 =         TOP,
        MACH3 =         TOP,
    };
};

__END_SYS

#endif
