// EPOS EPOSMoteIII (Cortex-M4) Mediators Configuration

#ifndef __emote3_config_h
#define __emote3_config_h

#include <system/meta.h>
#include __APPL_TRAITS_H

#define __CPU_H         __HEADER_ARCH(cpu)
#define __TSC_H         __HEADER_ARCH(tsc)
#define __MMU_H         __HEADER_ARCH(mmu)

#define __MACH_H        __HEADER_MACH(machine)
#define __MODEL_H       __HEADER_MACH(MMOD)
#define __IC_H          __HEADER_MACH(ic)
#define __TIMER_H       __HEADER_MACH(timer)
#define __RTC_H         __HEADER_MACH(rtc)
#define __EEPROM_H      __HEADER_MACH(eeprom)
#define __UART_H        __HEADER_MACH(uart)
//#define __DISPLAY_H     __HEADER_MACH(display)
#define __NIC_H         __HEADER_MACH(nic)
//#define __SCRATCHPAD_H  __HEADER_MACH(scratchpad)

__BEGIN_SYS

typedef ARMv7              CPU;
typedef ARMv7_MMU          MMU;
typedef ARMv7_TSC          TSC;

typedef Cortex_M             Machine;
typedef Cortex_M_IC          IC;
typedef Cortex_M_Timer       Timer;
typedef Cortex_M_RTC         RTC;
typedef Cortex_M_EEPROM      EEPROM;
typedef Cortex_M_UART        UART;
typedef IF<Traits<Serial_Display>::enabled, Serial_Display, Cortex_M_Display>::Result Display;
typedef Cortex_M_Radio       NIC;
typedef Cortex_M_Scratchpad  Scratchpad;

__END_SYS

#endif
