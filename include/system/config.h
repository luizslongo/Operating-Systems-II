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

// Dummy class for incomplete architectures and machines 
class Dummy;

// Architecture configuration
#if defined (__ia32)
#define ARCH ia32
typedef IA32 CPU;
#elif defined (__avr8)
#define ARCH avr8
typedef AVR8 CPU;
#else
#error Architecture not selected!
#endif


// Machine configuration
#if defined (__pc)
#define MACH pc
typedef PC Machine;
typedef Dummy ADC;
#elif defined (__avrmcu)
#define MACH avrmcu
typedef AVRMCU Machine;
typedef Dummy PCI;
#elif defined (__mica2)
#define MACH mica2
typedef Mica2 Machine;
typedef Dummy PCI;
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


