// EPOS-- Internal Type Management System

#ifndef __types_h
#define __types_h

__BEGIN_SYS

// System Info
class System_Info;

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

// Hardware Mediators - Time-Stamp Counters
class IA32_TSC;
class AVR8_TSC;

// Hardware Mediators - Memory Management Units
class IA32_MMU;
class AVR8_MMU;


// Hardware Mediators - Machines
class PC;
class ATMega16;
class ATMega128;

// Hardware Mediators - Busses
class PC_PCI;

// Hardware Mediators - Interrupt Controllers
class PC_IC;
class ATMega16_IC;
class ATMega128_IC;

// Hardware Mediators - Timers
class PC_Timer;
class ATMega16_Timer;
class ATMega128_Timer;

// Hardware Mediators - Real Time Clocks
class PC_RTC;
class ATMega16_RTC;
class ATMega128_RTC;

// Hardware Mediators - EEPROMs
class PC_EEPROM;
class ATMega16_EEPROM;
class ATMega128_EEPROM;

// Hardware Mediators - UARTs
class PC_UART;
class ATMega16_UART;
class ATMega128_UART;

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

// Hardware Mediators - ADCs
class ATMega16_ADC;
class ATMega128_ADC;

// Hardware Mediators - Sensors
class PC_Sensor;
class ATMega128_Temperature_Sensor;
class ATMega128_Light_Sensor;


// Abstractions	- Process
class Thread;
class Task;
class Active;

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
    LIGHT_SENSOR_ID,

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

//     TEMPERATURE_SENTIENT_ID,
//     PHOTO_SENTIENT_ID,

    UNKNOWN_TYPE_ID,
    LAST_TYPE_ID = UNKNOWN_TYPE_ID - 1
};


// Dummy class for incomplete architectures and machines 
template<int i>
class Dummy
{
public:
    static int init(System_Info * si) { return 0; }
};

__END_SYS

#endif
