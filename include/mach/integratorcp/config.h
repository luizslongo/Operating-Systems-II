#ifndef __integrator_config_h
#define __integrator_config_h

__BEGIN_SYS

typedef ARM7      CPU;
typedef ARM7_MMU  MMU;
typedef ARM7_TSC  TSC;

#define __CPU_H  __HEADER_ARCH(cpu)
#define __TSC_H  __HEADER_ARCH(tsc)
#define __MMU_H  __HEADER_ARCH(mmu)

typedef IntegratorCP          Machine;
typedef Dummy<0>              PCI;
typedef IntegratorCP_IC       IC;
typedef Dummy<1>              EEPROM;
typedef Dummy<2>              Flash;
typedef IntegratorCP_Timer<0> Alarm_Timer;
typedef IntegratorCP_Timer<1> Scheduler_Timer;
typedef IntegratorCP_Timer<2> Timer;
typedef IntegratorCP_UART     UART;
typedef	Dummy<3>             SPI;
typedef Serial_Display        Display;
typedef IntegratorCP_NIC      NIC;
typedef Dummy<5>              ADC;
typedef Dummy<6>              Temperature_Sensor;
typedef Dummy<7>              Photo_Sensor;
typedef IntegratorCP_RTC      RTC;
typedef Dummy<8>              CAN;
typedef Dummy<9>              Accelerometer;

#define __MACH_H     __HEADER_MACH(machine)
#define __IC_H       __HEADER_MACH(ic)
#define __TIMER_H    __HEADER_MACH(timer)
#define __UART_H     __HEADER_MACH(uart)
#define __NIC_H      __HEADER_MACH(nic)
#define __RTC_H      __HEADER_MACH(rtc)

__END_SYS

#endif
