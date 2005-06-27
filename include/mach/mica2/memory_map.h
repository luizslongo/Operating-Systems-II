// EPOS--  Memory Map for the Mica2

#ifndef __memory_map_mica2_h
#define __memory_map_mica2_h

__BEGIN_SYS

template <>
struct Memory_Map<Mica2>
{
    enum {
	MEM_BASE =	0,
	MEM_SIZE =	1024,
    };

    enum {
	BASE =		0x0000,
	TOP =		0xffff,
	APP_LO =	0x0000,
	APP_CODE =	0x0000,
	APP_DATA =	0x0000,
	APP_HI =	0x10ff,
	PHY_MEM =	0x0000,
	IO_MEM =	0x0020,
	SYS =		0x0000,
	INT_VEC =	0x0000,
	SYS_PT =	TOP,
	SYS_PD =	TOP,
	SYS_INFO =	0x0800,
	SYS_CODE =	0x0000,
	SYS_DATA =	0x0000,
	SYS_STACK =	0x10ff,
	MACH1 =		0x0000,
	MACH2 =		0x0000,
	MACH3 =		TOP,
    };
};

__END_SYS

#endif
