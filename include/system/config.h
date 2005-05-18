// EPOS-- Configuration Engine
//

#ifndef __config_h
#define __config_h

//============================================================================
// DEFINITIONS
//============================================================================
#define __SYS_NS		System
#define __BEGIN_SYS		namespace __SYS_NS {
#define __END_SYS		}
#define __USING_SYS		using namespace __SYS_NS;
#define __SYS(X)		::__SYS_NS::X

#define ASM                     __asm__
#define ASMV                    __asm__ __volatile__

//============================================================================
// CONFIGURATION
//============================================================================
#include <system/types.h>

__BEGIN_SYS

// Architecture configuration
#if defined (__ia32)
#define ARCH ia32
typedef IA32 CPU;
typedef IA32_TSC TSC;
typedef IA32_MMU MMU;
#elif defined (__avr8)
#define ARCH avr8
typedef AVR8 CPU;
typedef AVR8_TSC TSC;
typedef AVR8_MMU MMU;
#else
#error Architecture not selected!
#endif


// Machine configuration
#if defined (__pc)
#define MACH pc
typedef PC Machine;
typedef PC_IC IC;
typedef PC_Timer Timer;
typedef PC_RTC RTC;
typedef PC_PCI PCI;
typedef PC_Display Display;
#elif defined (__avrmcu)
#define MACH avrmcu
typedef AVRMCU Machine;
typedef AVRMCU_IC IC;
typedef AVRMCU_Timer Timer;
typedef AVRMCU_RTC RTC;
typedef AVRMCU_Display Display;
#else
#error Machine not selected!
#endif

__END_SYS

#define __HEADER_ARCH(X)        <arch/ARCH/X.h>
#define __HEADER_MACH(X)        <mach/MACH/X.h>

//============================================================================
// THINGS EVERBODY NEEDS
//============================================================================
__BEGIN_SYS
inline void * operator new(unsigned int s, void * a) { return a; }
__END_SYS

#include <traits.h>
#include <system/info.h>
#include <utility/ostream.h>
#include <utility/debug.h>
#include <cpu.h>

#endif


