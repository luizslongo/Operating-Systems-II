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
class Application;

// Hardware Mediators - CPU
class IA32;
class AVR8;
class PPC32;
class MIPS32;
class ARM7;

// Hardware Mediators - Time-Stamp Counter
class IA32_TSC;
class AVR8_TSC;
class PPC32_TSC;
class MIPS32_TSC;
class ARM7_TSC;

// Hardware Mediators - Memory Management Unit
class IA32_MMU;
class AVR8_MMU;
class PPC32_MMU;
class MIPS32_MMU;
class ARM7_MMU;

// Hardware Mediators - Performance Monitoring Unit
class IA32_PMU;

// Hardware Mediators - Machine
class PC;
class ATMega16;
class ATMega128;
class ATMega1281;
class AT90CAN128;
class ML310;
class PLASMA;
class AXI4LITE;
class MC13224V;
class IntegratorCP;

// Hardware Mediators - Bus
class PC_PCI;
class ML310_PCI;

// Hardware Mediators - Interrupt Controller
class PC_IC;
class ATMega16_IC;
class ATMega128_IC;
class ATMega1281_IC;
class AT90CAN128_IC;
class ML310_IC;
class PLASMA_IC;
class AXI4LITE_IC;
class MC13224V_IC;
class IntegratorCP_IC;

// Hardware Mediators - Timer
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
class AXI4LITE_Timer;
class MC13224V_Timer_0;
class MC13224V_Timer_1;
class MC13224V_Timer_2;
class MC13224V_Timer_3;
class IntegratorCP_Timer;

// Hardware Mediators - CAN
class AT90CAN128_CAN;

// Hardware Mediators - Real-time Clock
class PC_RTC;
class ATMega16_RTC;
class ATMega128_RTC;
class ATMega1281_RTC;
class AT90CAN128_RTC;
class ML310_RTC;
class PLASMA_RTC;
class AXI4LITE_RTC;
class MC13224V_RTC;
class IntegratorCP_RTC;

// Hardware Mediators - EEPROM
class PC_EEPROM;
class ATMega16_EEPROM;
class ATMega128_EEPROM;
class ATMega1281_EEPROM;

// Hardware Mediators - FLASH
class ATMega128_Flash;
class ATMega1281_Flash;
class MC13224V_Flash;
class IntegratorCP_Flash;

// Hardware Mediators - UART
class PC_UART;
class ATMega16_UART;
class ATMega128_UART;
class ATMega1281_UART;
class AT90CAN128_UART;
class ML310_UART;
class PLASMA_UART;
class AXI4LITE_UART;
class MC13224V_UART;
class IntegratorCP_UART;

// Hardware Mediators - USART
class ATMega1281_USART;

// Hardware Mediators - SPI
class PC_SPI;
class ATMega16_SPI;
class ATMega128_SPI;
class ATMega1281_SPI;

// Hardware Mediators - Display
class Serial_Display;
class PC_Display;
class ATMega16_Display;
class ATMega128_Display;
class ATMega1281_Display;
class AT90CAN128_Display;
class MC13224V_Display;

// Hardware Mediators - NIC
class PC_Ethernet;
class PCNet32;
class C905;
class E100;
class ATMega128_NIC;
class AT90CAN128_NIC;
class ATMega1281_NIC;
class ATMega128_Radio;
class ATMega1281_Radio;
class MC13224V_Radio;
class Radio_Wrapper;
class ML310_NIC;
class PLASMA_NIC;
class AXI4LITE_NIC;
class MC13224V_NIC;
class IntegratorCP_NIC;

// Hardware Mediators - GPIO
class MC13224V_GPIO_Pin;

// Hardware Mediators - ADC
class AT90CAN128_ADC;
class ATMega16_ADC;
class ATMega128_ADC;
class ATMega1281_ADC;
class AVR_ADC;
class MC13224V_ADC;
class MC13224V_Battery_ADC;

// Hardware Mediators - Battery
class ATMega1281_Battery;
class MC13224V_Battery;

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
class MC13224V_Temperature_Sensor;
class ERTJ1VG103FA;
class MC13224V_ADXL345;

// Hardware Mediators - Encryption
class AES_Controller;
class ASM_Controller;

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
    class GEDF;
    class GRM;
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

// Abstractions - Network
class Ethernet;
class Network;
class ELP;
class Router;
class IP;
class ICMP;
class UDP;
class TCP;
class RTP;

// Abstractions	- Sentient
class Photo_Sentient;
class Temperature_Sentient;

// Abstractions - IEEE 1451
class IEEE1451_NCAP;
class IEEE1451_TIM;
class IEEE1451_Transducer;

// Abstractions - SIP
class SIP_Body;
class SIP_Header;
class SIP_Manager;
class SIP_Message;
class SIP_Transaction;
class SIP_User_Agent;

// System Components IDs
// The order in this enumeration defines many things in the system (e.g. init)
typedef unsigned int Type_Id;
enum 
{
    CPU_ID,
    TSC_ID,
    MMU_ID,
        PMU_ID,

    MACHINE_ID,
    PCI_ID,
    NOC_ID,
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
    GPIO_PIN_ID,
    ADC_ID,
    BATTERY_ID,
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
