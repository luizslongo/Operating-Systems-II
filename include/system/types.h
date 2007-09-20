// EPOS-- Internal Type Management System

#ifndef __types_h
#define __types_h

inline void * operator new(unsigned int s, void * a) { return a; }

__BEGIN_SYS

// Dummy class for incomplete architectures and machines 
template<int i>
class Dummy;

// Utilities
class Debug;
class Heap;

// System parts
class Boot;
class Setup;
class Init;
class System;

// Hardware Mediators - CPUs
class IA32;
class AVR8;
class PPC32;
class MIPS32;

// Hardware Mediators - Time-Stamp Counters
class IA32_TSC;
class AVR8_TSC;
class PPC32_TSC;
class MIPS32_TSC;

// Hardware Mediators - Memory Management Units
class IA32_MMU;
class AVR8_MMU;
class PPC32_MMU;
class MIPS32_MMU;

// Hardware Mediators - Machines
class PC;
class ATMega16;
class ATMega128;
class ML310;
class PLASMA;

// Hardware Mediators - Busses
class PC_PCI;
class ML310_PCI;

// Hardware Mediators - Interrupt Controllers
class PC_IC;
class ATMega16_IC;
class ATMega128_IC;
class ML310_IC;
class PLASMA_IC;

// Hardware Mediators - Timers
class PC_Timer;
class ATMega16_Timer;
class ATMega128_Timer;
class ML310_Timer;
class PLASMA_Timer;

// Hardware Mediators - Real Time Clocks
class PC_RTC;
class ATMega16_RTC;
class ATMega128_RTC;
class ML310_RTC;
class PLASMA_RTC;

// Hardware Mediators - EEPROMs
class PC_EEPROM;
class ATMega16_EEPROM;
class ATMega128_EEPROM;

// Hardware Mediators - UARTs
class PC_UART;
class ATMega16_UART;
class ATMega128_UART;
class ML310_UART;
class PLASMA_UART;

// Hardware Mediators - SPIs
class PC_SPI;
class ATMega16_SPI;
class ATMega128_SPI;

// Hardware Mediators - Displays
class Serial_Display;
class PC_Display;
class ATMega16_Display;
class ATMega128_Display;

// Hardware Mediators - NICs
class PC_NIC;
class PCNet32;
class C905;
class E100;
class ATMega16_NIC;
class ATMega128_NIC;
class Radio;
class ML310_NIC;
class CMAC;
class PLASMA_NIC;

// Hardware Mediators - ADCs
class ATMega16_ADC;
class ATMega128_ADC;

// Hardware Mediators - Sensors
class PC_Sensor;
class ATMega128_Temperature_Sensor;
class ATMega128_Photo_Sensor;
class ATMega128_Accelerometer;
class MTS300;
class MTS300_Temperature;
class MTS300_Photo;
class ADXL202;
class Dummy_Accel;


// Abstractions	- Process
class Thread;
class Task;
class Active;

// Abstractions - Scheduler
template <typename> class Scheduler;
namespace Scheduling_Criteria
{
    class Priority;
    class FCFS;
    class Round_Robin;
    class RM;
    class EDF;
};

// Abstractions	- Memory
class Segment;
class Address_Space;

// Abstractions	- Synchronization
class Synchronizer;
class Mutex;
class Semaphore;
class Condition;

// Abstractions	- Time
class Clock;
class Alarm;
class Chronometer;

// Abstractions	- Communication
class Network;
class IP;
class ARP;
class UDP;
template <typename> class Channel;
typedef Channel<UDP> UDP_Channel;

// Abstractions	- Sentients
class Photo_Sentient;
class Temperature_Sentient;


// System Components IDs
// The order in this enumeration defines many things in the system (e.g. init)
typedef unsigned int Type_Id;
enum 
{
    CPU_ID,
    TSC_ID,
    MMU_ID,

    MACHINE_ID,
    PCI_ID,
    IC_ID,
    TIMER_ID,
    RTC_ID,
    EEPROM_ID,
    UART_ID,
    SPI_ID,
    DISPLAY_ID,
    NIC_ID,
    ADC_ID,
    TEMPERATURE_SENSOR_ID,
    PHOTO_SENSOR_ID,
    ACCELEROMETER_ID,

    THREAD_ID,
    TASK_ID,
    ACTIVE_ID,

    SEGMENT_ID,
    ADDRESS_SPACE_ID,

    MUTEX_ID,
    SEMAPHORE_ID,
    CONDITION_ID,

    CLOCK_ID,
    ALARM_ID,
    CHRONOMETER_ID,

    NETWORK_ID,
    IP_ID,
    ARP_ID,
    UDP_ID,

    UNKNOWN_TYPE_ID,
    LAST_TYPE_ID = UNKNOWN_TYPE_ID - 1
};

__END_SYS

#endif
