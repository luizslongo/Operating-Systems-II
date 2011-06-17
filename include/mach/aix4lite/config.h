// EPOS AIX4LITE Mediators Configuration

#ifndef __aix4lite_config_h
#define __aix4lite_config_h

#define __CPU_H				__HEADER_ARCH(cpu)
#define __TSC_H				__HEADER_ARCH(tsc)
#define __MMU_H				__HEADER_ARCH(mmu)

#define __MACH_H			__HEADER_MACH(machine)
#define __IC_H				__HEADER_MACH(ic)
#define __TIMER_H			__HEADER_MACH(timer)
#define __UART_H			__HEADER_MACH(uart)
#define __RTC_H				__HEADER_MACH(rtc)
#define __NIC_H				__HEADER_MACH(nic)

__BEGIN_SYS

typedef MIPS32				CPU;
typedef MIPS32_MMU			MMU;
typedef MIPS32_TSC			TSC;

typedef AIX4LITE			Machine;
typedef AIX4LITE_IC			IC;
typedef AIX4LITE_Timer  	Timer;
typedef AIX4LITE_RTC		RTC;
typedef AIX4LITE_UART		UART;
typedef Serial_Display		Display;
typedef AIX4LITE_NIC		NIC;

__END_SYS

#endif
