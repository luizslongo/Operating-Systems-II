#ifndef __panda_config_h
#define __panda_config_h
//#include<traits.h>
__BEGIN_SYS

typedef ARMV7      CPU;
typedef ARMV7_MMU  MMU;
typedef ARMV7_TSC  TSC;

#define __CPU_H  __HEADER_ARCH(cpu)
#define __TSC_H  __HEADER_ARCH(tsc)
#define __MMU_H  __HEADER_ARCH(mmu)

#define __MACH_H     __HEADER_MACH(machine)
#define __IC_H       __HEADER_MACH(ic)
#define __TIMER_H    __HEADER_MACH(timer)
#define __UART_H     __HEADER_MACH(uart)
#define __NIC_H      __HEADER_MACH(nic)
#define __RTC_H      __HEADER_MACH(rtc)

//typedef PandaBoard          Machine;
//typedef IF<Traits<Zynq>::enabled, Zynq, PandaBoard>::Result    Machine;
typedef Zynq Machine;
typedef Zynq_IC       IC;
//typedef IF<Traits<Zynq>::enabled, Zynq_Timer, PandaBoard_Timer>::Result    Timer;
//typedef IF<Traits<Zynq>::enabled, Zynq_UART, PandaBoard_UART>::Result     UART;
typedef Zynq_UART UART;
typedef Zynq_Timer Timer;
typedef Serial_Display      Display;
typedef PandaBoard_NIC      NIC;
typedef PandaBoard_RTC      RTC;

class TSC_Timer;

__END_SYS

#endif
