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

#define __HEADER_ARCH(X)        <arch/ARCH/X.h>
#define __HEADER_MACH(X)        <mach/MACH/X.h>

inline void * operator new(unsigned int s, void * a) { return a; }

//============================================================================
// CONFIGURATION
//============================================================================
#include <system/types.h>
#include <traits.h>

__BEGIN_SYS

// "If" metaprogram
template<bool condition, typename Then, typename Else>
struct If { typedef Then Result; };

template<typename Then, typename Else>
struct If<false, Then, Else> { typedef Else Result; };

// Dummy class for incomplete architectures and machines 
template<int i>
class Dummy
{
public:
    static int init(System_Info * si) { return 0; }
};

// Architecture configuration
#if defined (__ia32)

#define ARCH ia32
typedef IA32			CPU;
typedef IA32_MMU		MMU;
typedef IA32_TSC		TSC;
#define __CPU_H			__HEADER_ARCH(cpu)
#define __TSC_H			__HEADER_ARCH(tsc)
#define __MMU_H			__HEADER_ARCH(mmu)

#elif defined (__avr8)

#define ARCH avr8
typedef AVR8			CPU;
typedef AVR8_MMU		MMU;
typedef AVR8_TSC		TSC;
#define __CPU_H			__HEADER_ARCH(cpu)
#define __TSC_H			__HEADER_ARCH(tsc)
#define __MMU_H			__HEADER_ARCH(mmu)


#else
#error Architecture not selected!
#endif

// Machine configuration
#if defined (__pc)

#define MACH			pc
typedef PC			Machine;
typedef PC_PCI			PCI;
typedef PC_IC			IC;
typedef PC_Timer		Timer;
typedef PC_RTC			RTC;
typedef PC_EEPROM		EEPROM;
typedef PC_UART			UART;
typedef Dummy<0>		SPI;
typedef If<Traits<PC_Display>::on_serial,
	   Serial_Display,
	   PC_Display>::Result	Display;
typedef PC_NIC			NIC;
typedef Dummy<1>		ADC;
typedef Dummy<2>		Temperature_Sensor;
typedef Dummy<3>		Light_Sensor;
#define __MACH_H		__HEADER_MACH(pc)
#define __MEMORY_MAP_H		__HEADER_MACH(memory_map)
#define __PCI_H			__HEADER_MACH(pci)
#define __IC_H			__HEADER_MACH(ic)
#define __TIMER_H		__HEADER_MACH(timer)
#define __RTC_H			__HEADER_MACH(rtc)
#define __EEPROM_H		__HEADER_MACH(eeprom)
#define __UART_H		__HEADER_MACH(uart)
#define __DISPLAY_H		__HEADER_MACH(display)
#define __NIC_H			__HEADER_MACH(nic)

#elif defined (__atmega16)

#define MACH atmega16
typedef ATMega16		Machine;
typedef ATMega16_IC		IC;
typedef ATMega16_Timer		Timer;
typedef ATMega16_RTC		RTC;
typedef ATMega16_EEPROM		EEPROM;
typedef Dummy<0>		PCI;
typedef ATMega16_UART		UART;
typedef ATMega16_SPI		SPI;
typedef If<Traits<ATMega16_Display>::on_serial,
	   Serial_Display,
	   PC_Display>::Result	Display;
typedef ATMega16_NIC		NIC;
typedef ATMega16_ADC		ADC;
typedef Dummy<1>		Temperature_Sensor;
typedef Dummy<2>		Light_Sensor;
#define __MACH_H		__HEADER_MACH(atmega16)
#define __MEMORY_MAP_H		__HEADER_MACH(memory_map)
#define __IC_H			__HEADER_MACH(ic)
#define __TIMER_H		__HEADER_MACH(timer)
#define __RTC_H			__HEADER_MACH(rtc)
#define __EEPROM_H		__HEADER_MACH(eeprom)
#define __UART_H		__HEADER_MACH(uart)
#define __SPI_H			__HEADER_MACH(spi)
#define __NIC_H			__HEADER_MACH(nic)
#define __ADC_H			__HEADER_MACH(adc)

#elif defined (__atmega128)

#define MACH atmega128
typedef ATMega128		Machine;
typedef ATMega128_IC		IC;
typedef ATMega128_Timer		Timer;
typedef ATMega128_RTC		RTC;
typedef ATMega128_EEPROM	EEPROM;
typedef Dummy<0>		PCI;
typedef ATMega128_UART		UART;
typedef ATMega128_SPI		SPI;
typedef If<Traits<ATMega128_Display>::on_serial,
	   Serial_Display,
	   PC_Display>::Result	Display;
typedef ATMega128_NIC		NIC;
typedef ATMega128_ADC		ADC;
typedef ATMega128_Temperature_Sensor	Temperature_Sensor;
typedef ATMega128_Light_Sensor	Light_Sensor;
#define __MACH_H		__HEADER_MACH(atmega128)
#define __MEMORY_MAP_H		__HEADER_MACH(memory_map)
#define __IC_H			__HEADER_MACH(ic)
#define __TIMER_H		__HEADER_MACH(timer)
#define __RTC_H			__HEADER_MACH(rtc)
#define __EEPROM_H		__HEADER_MACH(eeprom)
#define __UART_H		__HEADER_MACH(uart)
#define __SPI_H			__HEADER_MACH(spi)
#define __NIC_H			__HEADER_MACH(nic)
#define __ADC_H			__HEADER_MACH(adc)
#define __SENSOR_H		__HEADER_MACH(sensor)

#else

#error Machine not selected!

#endif

__END_SYS

#include <system/ctti.h>

//============================================================================
// THINGS EVERBODY NEEDS
//============================================================================
#include <system/info.h>
#include <utility/ostream.h>
#include <utility/debug.h>
#include <cpu.h>

#endif


