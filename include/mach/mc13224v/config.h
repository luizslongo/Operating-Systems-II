// EPOS MC13224V Mediators Configuration

#ifndef __mc13224v_config_h
#define __mc13224v_config_h

#define __CPU_H				__HEADER_ARCH(cpu)
#define __TSC_H				__HEADER_ARCH(tsc)
#define __MMU_H				__HEADER_ARCH(mmu)

#define __PCI_H				__HEADER_MACH(pci)
#define __RTC_H				__HEADER_MACH(rtc)
#define __EEPROM_H			__HEADER_MACH(eeprom)
#define __NIC_H				__HEADER_MACH(nic)
#define __MACH_H			__HEADER_MACH(machine)
#define __IC_H				__HEADER_MACH(ic)
#define __TIMER_H			__HEADER_MACH(timer)
#define __UART_H			__HEADER_MACH(uart)
#define __FLASH_H			__HEADER_MACH(flash)
#define __ADC_H				__HEADER_MACH(adc)

__BEGIN_SYS

typedef ARM7		CPU;
typedef ARM7_MMU	MMU;
typedef ARM7_TSC	TSC;

typedef MC13224V		Machine;
typedef MC13224V_IC		IC;
typedef MC13224V_Flash 		Flash;
typedef MC13224V_Timer_0	Alarm_Timer;
typedef MC13224V_Timer_1	Scheduler_Timer;
typedef MC13224V_Timer_1    Timer_1;
typedef Alarm_Timer		Timer;
typedef MC13224V_UART		UART;
typedef Serial_Display		Display;
typedef MC13224V_NIC		NIC;
typedef MC13224V_ADC		ADC;
typedef MC13224V_RTC		RTC;


__END_SYS

#endif

