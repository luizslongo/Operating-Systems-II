#ifndef __atmega1281_traits_h
#define __atmega1281_traits_h

#include "../avr_common/traits.h"

__BEGIN_SYS

class ATMega1281_Common;
template <> struct Traits<ATMega1281_Common>: public Traits<AVR_Common>
{
};

template <> struct Traits<ATMega1281>: public Traits<ATMega1281_Common>, public Traits<AVR_Machine>
{
    static const unsigned int MAX_CPUS = 1;
    static const unsigned long long CLOCK     = 8000000;
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;

    static const unsigned int APPLICATION_STACK_SIZE = 256; 
    static const unsigned int APPLICATION_HEAP_SIZE  = 2048;

    static const unsigned int SYSTEM_STACK_SIZE = 0;   // not used
    static const unsigned int SYSTEM_HEAP_SIZE  = 4096; 
};

template <> struct Traits<ATMega1281_Timer_1>: public Traits<ATMega1281_Common>
{
    static const int FREQUENCY = 1000; // Hz
};

template <> struct Traits<ATMega1281_Timer_2>: public Traits<ATMega1281_Common>
{
    static const int FREQUENCY = 100; // Hz
};

template <> struct Traits<ATMega1281_Timer_3>: public Traits<ATMega1281_Common>
{
    static const int FREQUENCY = 100; // Hz
};

template <> struct Traits<ATMega1281_RTC>: public Traits<ATMega1281_Common>
{
    static const unsigned int EPOCH_DAY   = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR  = 1970;
    static const unsigned int EPOCH_DAYS  = 719499;
};

template <> struct Traits<ATMega1281_EEPROM>: public Traits<ATMega1281_Common>
{
    static const unsigned int SIZE = 4096; // bytes
};

template <> struct Traits<ATMega1281_Flash>: public Traits<ATMega1281_Common>
{
    static const unsigned int PAGE_SIZE = 256; // bytes
};

template <> struct Traits<ATMega1281_UART>: public Traits<ATMega1281_Common>, public Traits<AVR_UART>
{
    static const unsigned int DEFAULT_UNIT = 1;
};

template <> struct Traits<ATMega1281_ADC>: public Traits<ATMega1281_Common>, public Traits<AVR_ADC>
{
};

template <> struct Traits<ATMega1281_SPI>: public Traits<ATMega1281_Common>
{
};

template <> struct Traits<ATMega1281_NIC>: public Traits<void>
{
    static const bool enabled = true;

    typedef LIST<CMAC> NICS;

    static const unsigned int RADIO_UNITS = NICS::Count<Radio>::Result;
};

template <> struct Traits<CMAC>: public Traits<void>
{
    static const bool debugged      = false;
    static const bool SM_STEP_DEBUG = false;

    static const unsigned int  FREQUENCY       = 0;
    static const unsigned int  POWER           = 5;
    static const bool          TIME_TRIGGERED  = false;
    static const bool          COORDINATOR     = false;
    static const unsigned int  SLEEPING_PERIOD = 1000; //ms
    static const unsigned long TIMEOUT         = 1000; //ms
    static const unsigned int  ADDRESS         = 0x0001;
    static const unsigned int  MTU             = 118; 

    typedef CMAC_States::Generic_Sync		Sync_State;
    typedef CMAC_States::IEEE802154_Pack	Pack_State;
    typedef CMAC_States::Contention_Empty	Contention_State;
    typedef CMAC_States::Generic_Tx		Tx_State;
    typedef CMAC_States::IEEE802154_Ack_Rx	Ack_Rx_State;
    typedef CMAC_States::Generic_Lpl		Lpl_State;
    typedef CMAC_States::Generic_Rx		Rx_State;
    typedef CMAC_States::IEEE802154_Unpack	Unpack_State;
    typedef CMAC_States::IEEE802154_Ack_Tx	Ack_Tx_State;
};

template <> struct Traits<ATMega1281_Temperature_Sensor>: public Traits<void>
{
    typedef LIST<SHT11_Temperature> SENSORS;
};

template <> struct Traits<ATMega1281_Humidity_Sensor>: public Traits<void>
{
    typedef LIST<SHT11_Humidity> SENSORS;
};

__END_SYS

#endif
