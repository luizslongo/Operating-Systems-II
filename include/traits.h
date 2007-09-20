#ifndef __traits_h
#define __traits_h

#include <system/config.h>

__BEGIN_SYS

template <class Imp>
struct Traits
{
    static const bool enabled = true;
    static const bool debugged = true;
};


// Utilities
template <> struct Traits<Debug>
{
    static const bool error   = true;
    static const bool warning = true;
    static const bool info    = false;
    static const bool trace   = false;
};

template <> struct Traits<Heap>: public Traits<void>
{
};


// System parts
template <> struct Traits<Boot>: public Traits<void>
{
};

template <> struct Traits<Setup>: public Traits<void>
{
};

template <> struct Traits<Init>: public Traits<void>
{
};

template <> struct Traits<System>: public Traits<void>
{
};


// Mediators - Architecture - IA32
#ifdef __ia32
template <> struct Traits<IA32>: public Traits<void>
{
};

template <> struct Traits<IA32_TSC>: public Traits<void>
{
};

template <> struct Traits<IA32_MMU>: public Traits<void>
{
};
#endif

// Mediators - Architecture - AVR8
#ifdef __avr8
template <> struct Traits<AVR8>: public Traits<void>
{
    static const bool Power_Management = false;
};

template <> struct Traits<AVR8_TSC>: public Traits<void>
{
};

template <> struct Traits<AVR8_MMU>: public Traits<void>
{
};
#endif

// Mediators - Architecture - PPC32
#ifdef __ppc32
template <> struct Traits<PPC32>: public Traits<void>
{
};

template <> struct Traits<PPC32_TSC>: public Traits<void>
{
};

template <> struct Traits<PPC32_MMU>: public Traits<void>
{
};
#endif

// Mediators - Architecture - MIPS32
#ifdef __mips32
template <> struct Traits<MIPS32>: public Traits<void>
{
};

template <> struct Traits<MIPS32_TSC>: public Traits<void>
{
};

template <> struct Traits<MIPS32_MMU>: public Traits<void>
{
};
#endif

// Mediators - Machine - Common
template <> struct Traits<Serial_Display>: public Traits<void>
{
    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 8;
};

// Mediators - Machine - PC
#ifdef __pc
class PC_Common;
template <> struct Traits<PC_Common>: public Traits<void>
{
};

template <> struct Traits<PC>: public Traits<PC_Common>
{
    static const unsigned int CLOCK = 797886000;

    static const unsigned int BOOT_IMAGE_ADDR = 0x00008000;
    static const unsigned int HARDWARE_INT_OFFSET = 0x20;
    static const unsigned int SYSCALL_INT = 0x80;

    static const unsigned int APPLICATION_STACK_SIZE = 16 * 1024;
    static const unsigned int APPLICATION_HEAP_SIZE = 16 * 1024 * 1024;

    static const unsigned int SYSTEM_STACK_SIZE = 4096;
    static const unsigned int SYSTEM_HEAP_SIZE = 16 * APPLICATION_STACK_SIZE;
};

template <> struct Traits<PC_PCI>: public Traits<PC_Common>
{
    static const int MAX_BUS = 0;
    static const int MAX_DEV_FN = 0xff;
};

template <> struct Traits<PC_IC>: public Traits<PC_Common>
{
};

template <> struct Traits<PC_Timer>: public Traits<PC_Common>
{
    // Meaningful values for the PC's timer frequency range from 100 to 
    // 10000 Hz. The choice must respect the scheduler time-slice, i. e.,
    // it must be higher than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template <> struct Traits<PC_RTC>: public Traits<PC_Common>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<PC_EEPROM>: public Traits<PC_Common>
{
};

template <> struct Traits<PC_UART>: public Traits<PC_Common>
{
    static const unsigned int CLOCK = 1843200; // 1.8432 MHz
    static const unsigned int COM1 = 0x3f8; // to 0x3ff, IRQ4
    static const unsigned int COM2 = 0x2f8; // to 0x2ff, IRQ3
    static const unsigned int COM3 = 0x3e8; // to 0x3ef, no IRQ
    static const unsigned int COM4 = 0x2e8; // to 0x2ef, no IRQ
};

template <> struct Traits<PC_Display>: public Traits<PC_Common>
{
    static const bool on_serial = true;
    static const int COLUMNS = 80;
    static const int LINES = 25;
    static const int TAB_SIZE = 8;
    static const unsigned int FRAME_BUFFER_ADDRESS = 0xb8000;
};

template <> struct Traits<PC_NIC>: public Traits<PC_Common>
{
    typedef LIST<PCNet32, PCNet32> NICS;

    static const bool INT_ON_RECEIVE = true;

    static const unsigned int PCNET32_UNITS = NICS::Count<PCNet32>::Result;
    static const unsigned int PCNET32_SEND_BUFFERS = 8; // per unit
    static const unsigned int PCNET32_RECEIVE_BUFFERS = 8; // per unit

    static const unsigned int E100_UNITS = NICS::Count<E100>::Result;
    static const unsigned int E100_SEND_BUFFERS = 8; // per unit
    static const unsigned int E100_RECEIVE_BUFFERS = 8; // per unit

    static const unsigned int C905_UNITS = NICS::Count<C905>::Result;
    static const unsigned int C905_SEND_BUFFERS = 8; // per unit
    static const unsigned int C905_RECEIVE_BUFFERS = 8; // per unit
};
#endif

// Mediators - Machine - ATMega16
#ifdef __atmega16
class ATMega16_Common;
template <> struct Traits<ATMega16_Common>: public Traits<void>
{
};

template <> struct Traits<ATMega16>: public Traits<ATMega16_Common>
{
    static const unsigned long long CLOCK = 8000000;
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;

    static const unsigned int APPLICATION_STACK_SIZE = 128;
    static const unsigned int APPLICATION_HEAP_SIZE = 256;

    static const unsigned int SYSTEM_STACK_SIZE = 64;
    static const unsigned int SYSTEM_HEAP_SIZE = 4 *  APPLICATION_STACK_SIZE;
};

template <> struct Traits<ATMega16_Timer>: public Traits<ATMega16_Common>
{
    static const int FREQUENCY = 800; // Hz
};

template <> struct Traits<ATMega16_RTC>: public Traits<ATMega16_Common>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<ATMega16_EEPROM>: public Traits<ATMega16_Common>
{
    static const unsigned int SIZE = 512; // bytes
};

template <> struct Traits<ATMega16_UART>: public Traits<void>
{
    enum {
        FULL                = 0,//Tx AND Rx enabled
	LIGHT               = 1,//Only Tx enabled
	STANDBY             = 2,//Only Rx enabled
	OFF                 = 3 //Tx AND Rx disabled
    };
    static const char Power_Management = FULL;
};

template <> struct Traits<ATMega16_ADC>: public Traits<void>
{
};

template <> struct Traits<ATMega16_SPI>: public Traits<ATMega16_Common>
{
};

template <> struct Traits<ATMega16_Display>: public Traits<ATMega16_Common>
{
    static const bool on_serial = true;
};

template <> struct Traits<ATMega16_NIC>: public Traits<void>
{
    static const bool enabled = false;

    typedef LIST<Radio> NICS;

    static const unsigned int RADIO_UNITS = NICS::Count<Radio>::Result;
};
#endif


// Mediators - Machine - ATMega128
#ifdef __atmega128
class ATMega128_Common;
template <> struct Traits<ATMega128_Common>: public Traits<void>
{
};

template <> struct Traits<ATMega128>: public Traits<ATMega128_Common>
{
    static const unsigned long long CLOCK = 7372800; //Mica2
    /*static const unsigned long long CLOCK = 8000000;*/
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;

    static const unsigned int APPLICATION_STACK_SIZE = 256;
    static const unsigned int APPLICATION_HEAP_SIZE = 512;

    static const unsigned int SYSTEM_STACK_SIZE = 64;
    static const unsigned int SYSTEM_HEAP_SIZE = 4 *  APPLICATION_STACK_SIZE;

};

template <> struct Traits<ATMega128_Timer>: public Traits<ATMega128_Common>
{
    // Should be between 30 and 7200 Hz
    static const int FREQUENCY = 720; // Hz
    
};

template <> struct Traits<ATMega128_RTC>: public Traits<ATMega128_Common>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<ATMega128_EEPROM>: public Traits<ATMega128_Common>
{
    static const unsigned int SIZE = 4096; // bytes
};

template <> struct Traits<ATMega128_UART>: public Traits<void>
{
    enum {
        FULL                = 0,//Tx AND Rx enabled
	LIGHT               = 1,//Only Tx enabled
	STANDBY             = 2,//Only Rx enabled
	OFF                 = 3 //Tx AND Rx disabled
    };
    static const char Power_Management = FULL;
};

template <> struct Traits<ATMega128_ADC>: public Traits<void>
{
};

template <> struct Traits<ATMega128_SPI>: public Traits<ATMega128_Common>
{
};

template <> struct Traits<ATMega128_Display>: public Traits<ATMega128_Common>
{
    static const bool on_serial = true;
};

template <> struct Traits<ATMega128_NIC>: public Traits<void>
{
    typedef LIST<CMAC> NICS;

    static const unsigned int RADIO_UNITS = NICS::Count<Radio>::Result;
};

template <> struct Traits<CMAC>: public Traits<void>
{
    static const unsigned int FREQUENCY = 0;
    static const unsigned int POWER = 5;
};

template <> struct Traits<ATMega128_Temperature_Sensor>: public Traits<void>
{
    typedef LIST<MTS300_Temperature> SENSORS;
};

template <> struct Traits<ATMega128_Photo_Sensor>: public Traits<void>
{
    typedef LIST<MTS300_Photo> SENSORS;
};

template <> struct Traits<ATMega128_Accelerometer>: public Traits<void>
{
    typedef LIST<ADXL202> SENSORS;
};


#endif

// Mediators - Machine - ML310
#ifdef __ml310
class ML310_Common;
template <> struct Traits<ML310_Common>: public Traits<void>
{
};

template <> struct Traits<ML310>: public Traits<ML310_Common>
{
    static const unsigned int CLOCK = 200000000;

    static const unsigned int BOOT_IMAGE_ADDR = 0x03000000;
    static const unsigned int SETUP_ADDR = 0x00600000;
    static const int INT_BASE = 0x03FF0000;
    static const unsigned int HARDWARE_INT_OFFSET = 0x20; //Verify!!!!!
    static const int LEDS_BASEADDR = 0x40040000; //GPIO LEDS

    static const unsigned int APPLICATION_STACK_SIZE = 16 * 1024;
    static const unsigned int APPLICATION_HEAP_SIZE = 16 * 1024 * 1024;

    static const unsigned int SYSTEM_STACK_SIZE = 4 * 1024;
    static const unsigned int SYSTEM_HEAP_SIZE = 16 * APPLICATION_STACK_SIZE;

};

template <> struct Traits<ML310_PCI>: public Traits<ML310_Common>
{
    static const bool enabled = false;

    static const int MAX_BUS = 1;
    static const int MAX_DEV_FN = 0xff;
    static const int BASE_ADDRESS = 0x3C000000;
    static const unsigned int HOST_BRIDGE_DEV = 8;
    static const unsigned int IO_SPACE_UPPER  = 0x3BFFFFFF;
    static const unsigned int IO_SPACE_LIMIT  = 0x3BFF0000;
    static const unsigned int MEM_SPACE_UPPER = 0x37FFFFFF;
    static const unsigned int MEM_SPACE_LIMIT = 0x20000000;
};

template <> struct Traits<ML310_IC>: public Traits<ML310_Common>
{
    static const int BASE_ADDRESS = 0x41200000;
};

template <> struct Traits<ML310_Timer>: public Traits<ML310_Common>
{
    // Meaningful values for the timer frequency range from 100 to 
    // 10000 Hz. The choice must respect the scheduler time-slice, i. e.,
    // it must be higher than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template <> struct Traits<ML310_RTC>: public Traits<ML310_Common>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<ML310_UART>: public Traits<ML310_Common>
{
    static const unsigned int CLOCK = 1843200; // 1.8432 MHz - Verify !
    static const unsigned int BASE_ADDRESS = 0x40400000;
};

template <> struct Traits<ML310_NIC>: public Traits<ML310_Common>
{
    static const bool enabled = false;
    
    typedef LIST<E100> NICS;

    static const unsigned int E100_UNITS = NICS::Count<E100>::Result;
    static const unsigned int E100_SEND_BUFFERS = 32; // per unit
    static const unsigned int E100_RECEIVE_BUFFERS = 32; // per unit
};
#endif

// Mediators - Machine - PLASMA
#ifdef __plasma
class PLASMA_Common;
template <> struct Traits<PLASMA_Common>: public Traits<void>
{
};

template <> struct Traits<PLASMA>: public Traits<PLASMA_Common>
{
    static const unsigned int CLOCK                  = 25000000;
    static const unsigned int INT_VECTOR_ADDRESS     = 0x0000003C;

    static const unsigned int APPLICATION_STACK_SIZE = 16 * 1024;
    static const unsigned int APPLICATION_HEAP_SIZE  = 16 * 1024;
    static const unsigned int SYSTEM_STACK_SIZE      = 4 * 1024;
    static const unsigned int SYSTEM_HEAP_SIZE       = 16 * APPLICATION_STACK_SIZE;
    static const unsigned int LEDS_ADDRESS           = 0x20000030;
};

template <> struct Traits<PLASMA_IC>: public Traits<PLASMA_Common>
{
    static const unsigned int BASE_ADDRESS   = 0x20000010;
    static const unsigned int MASK_ADDRESS   = BASE_ADDRESS;
    static const unsigned int STATUS_ADDRESS = BASE_ADDRESS + 0x0010;
};

template <> struct Traits<PLASMA_UART>: public Traits<PLASMA_Common>
{
    static const unsigned int CLOCK        = 1843200; // 1.8432 MHz - Verify !
    static const unsigned int BASE_ADDRESS = 0x20000000;
    static const unsigned int DATA_ADDRESS = BASE_ADDRESS;
    static const unsigned int STATUS_ADDRESS = Traits<PLASMA_IC>::STATUS_ADDRESS;
};

template <> struct Traits<PLASMA_Timer>: public Traits<PLASMA_Common>
{
    static const unsigned int FREQUENCY = Traits<PLASMA>::CLOCK / (1<<18);
    static const unsigned int BASE_ADDRESS = 0x20000060;
    static const unsigned int DATA_ADDRESS = BASE_ADDRESS;
    static const unsigned int WRITE_ADDRESS = BASE_ADDRESS + 0x0010;
};

template <> struct Traits<PLASMA_NIC>: public Traits<void>
{
    static const bool enabled = false;
};

#endif

// Abstractions
template <> struct Traits<Thread>: public Traits<void>
{
    typedef Scheduling_Criteria::Priority Criterion;
    static const bool idle_waiting = true;
    static const bool active_scheduler = true;
    static const bool preemptive = true;
    static const bool smp = false;
    static const unsigned int QUANTUM = 10000; // us
};

template <> struct Traits<Scheduler<Thread> >: public Traits<void>
{
};

template <> struct Traits<Alarm>: public Traits<void>
{
    static const bool visible = false;
};

template <> struct Traits<Synchronizer>: public Traits<void>
{
    static const bool strict_ordering = true;
};

template <> struct Traits<Network>: public Traits<void>
{
    static const unsigned int EPOS_LIGHT_PROTOCOL = 0x8888;
};

template <> struct Traits<ARP>: public Traits<void>{
        static const unsigned int TRIES = 3; // # of attempts for an ARP query
        static const unsigned int TIMEOUT = 1000000; // 1s
};

template <> struct Traits<IP>: public Traits<void>{
        static const unsigned int ADDRESS = 0xc0a80a01;   // 192.168.10.1
        static const unsigned int NETMASK = 0xffffff00;   // 255.255.255.0
        static const unsigned int BROADCAST = 0; // 0= Default Broadcast Address
};


__END_SYS

#endif
