// EPOS--  Memory Map for the AVRMCU

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.


#ifndef __memory_map_avrmcu_h
#define __memory_map_avrmcu_h

__BEGIN_SYS

template <>
struct Memory_Map<AVRMCU>
{
    enum {
	MEM_BASE =		0,
	MEM_SIZE =		17408,
    };

    enum {
	BASE =		0x0000,
	TOP =		0xffff,
	APP_LO =	0x0000,
	APP_CODE =	0x0000,
	APP_DATA =	0x0000,
	APP_HI =	0x037e,
	PHY_MEM =	0x00000000,
	IO_MEM =	0x0000,
	SYS =		0x0000,
	INT_VEC =	0x0000,
	SYS_PT =	TOP,
	SYS_PD =	TOP,
	SYS_INFO =	0x0400,
	SYS_CODE =	0x0000,
	SYS_DATA =	0x0000,
	SYS_STACK =	0x03fe,
	MACH1 =		0x0000,
	MACH2 =		0x0000,
	MACH3 =		TOP,
    };
};

__END_SYS

#endif
