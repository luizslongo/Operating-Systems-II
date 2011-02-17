// EPOS Internal Type Management System

typedef __SIZE_TYPE__ size_t;

#ifndef __types_h
#define __types_h

inline void * operator new(size_t s, void * a) { return a; }
inline void * operator new[](size_t s, void * a) { return a; }

__BEGIN_SYS

// Dummy class for incomplete architectures and machines 
template<int>
class Dummy;

// Utilities
class Debug;
class Lists;
class Spin;
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

/*
namespace Cpu {
    template<bool, bool>
    class AVR8_CPU;
};*/

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
class ATMega1281;
class AT90CAN128;
class ML310;
class PLASMA;

// Hardware Mediators - Busses
class PC_PCI;
class ML310_PCI;

// Hardware Mediators - Interrupt Controllers
class PC_IC;
class ATMega16_IC;
class ATMega128_IC;
class ATMega1281_IC;
class AT90CAN128_IC;
class ML310_IC;
class PLASMA_IC;

// Hardware Mediators - Timers
class PC_Timer;
class ATMega16_Timer_1;
class ATMega16_Timer_2;
class ATMega16_Timer_3;
class ATMega128_Timer_1;
class ATMega128_Timer_2;
class ATMega128_Timer_3;
class ATMega1281_Timer_1;
class ATMega1281_Timer_2;
class ATMega1281_Timer_3;
class AT90CAN128_Timer_1;
class AT90CAN128_Timer_2;
class AT90CAN128_Timer_3;
class ML310_Timer;
class PLASMA_Timer;

// Hardware Mediators - CAN
class AT90CAN128_CAN;

// Hardware Mediators - Real Time Clocks
class PC_RTC;
class ATMega16_RTC;
class ATMega128_RTC;
class ATMega1281_RTC;
class AT90CAN128_RTC;
class ML310_RTC;
class PLASMA_RTC;

// Hardware Mediators - EEPROMs
class PC_EEPROM;
class ATMega16_EEPROM;
class ATMega128_EEPROM;
class ATMega1281_EEPROM;

// Hardware Mediators - FLASHs
class ATMega128_Flash;
class ATMega1281_Flash;

// Hardware Mediators - UARTs
class PC_UART;
class ATMega16_UART;
class ATMega128_UART;
class ATMega1281_UART;
class AT90CAN128_UART;
class ML310_UART;
class PLASMA_UART;

// Hardware Mediators - USARTs
class ATMega1281_USART;

// Hardware Mediators - SPIs
class PC_SPI;
class ATMega16_SPI;
class ATMega128_SPI;
class ATMega1281_SPI;

// Hardware Mediators - Displays
class Serial_Display;
class PC_Display;
class ATMega16_Display;
class ATMega128_Display;
class ATMega1281_Display;
class AT90CAN128_Display;

// Hardware Mediators - NICs
class PC_Ethernet;
class PCNet32;
class C905;
class E100;
class ATMega16_NIC;
class ATMega128_NIC;
class AT90CAN128_NIC;
class ATMega1281_NIC;
class ATMega128_Radio;
class ATMega1281_Radio;
class ML310_NIC;
class PLASMA_NIC;
class CMAC;
namespace CMAC_States
{
    class Empty;
    class Sync_Empty;
    class Pack_Empty;
    class Contention_Empty;
    class Tx_Empty;
    class Ack_Rx_Empty;
    class Lpl_Empty;
    class Rx_Empty;
    class Unpack_Empty;
    class Ack_Tx_Empty;

    class Generic_Sync;
    class Generic_Tx;
    class Generic_Rx;
    class Generic_Lpl;
    class Unslotted_CSMA_Contention;

    class IEEE802154_Beacon_Sync;
    class IEEE802154_Pack;
    class IEEE802154_Unpack;
    class IEEE802154_Ack_Rx;
    class IEEE802154_Ack_Tx;
    class IEEE802154_Slotted_CSMA_Contention;
};

// Hardware Mediators - ADCs
class AT90CAN128_ADC;
class ATMega16_ADC;
class ATMega128_ADC;
class ATMega1281_ADC;

// Hardware Mediators - Sensors
class PC_Sensor;
class ATMega128_Temperature_Sensor;
class ATMega128_Photo_Sensor;
class ATMega128_Accelerometer;
class ATMega1281_Temperature_Sensor;
class ATMega1281_Humidity_Sensor;
class MTS300;
class MTS300_Temperature;
class MTS300_Photo;
class ADXL202;
class Dummy_Accel;
class SHT11;
class SHT11_Temperature;
class SHT11_Humidity;

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
    class EARM;
    class EAEDF;
    class CPU_Affinity;
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
class Ethernet;
template <typename, typename> class ARP;
class Network;
class IP;
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
    FLASH_ID,
    UART_ID,
    USART_ID,
    SPI_ID,
    DISPLAY_ID,
    NIC_ID,
    RADIO_ID,
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
    CAN_ID,

    UNKNOWN_TYPE_ID,
    LAST_TYPE_ID = UNKNOWN_TYPE_ID - 1
};

__END_SYS

#endif
