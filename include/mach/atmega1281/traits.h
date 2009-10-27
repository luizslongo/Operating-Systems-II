#ifndef __atmega1281_traits_h
#define __atmega1281_traits_h

__BEGIN_SYS

class ATMega1281_Common;
template <> struct Traits<ATMega1281_Common>: public Traits<void>
{
};

template <> struct Traits<ATMega1281>: public Traits<ATMega1281_Common>
{
    static const unsigned int MAX_CPUS = 1;
    static const unsigned long long CLOCK     = 1000000;
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;

    static const unsigned int APPLICATION_STACK_SIZE = 512; 
    static const unsigned int APPLICATION_HEAP_SIZE  = 2048;

    static const unsigned int SYSTEM_STACK_SIZE = 0;   // not used
    static const unsigned int SYSTEM_HEAP_SIZE  = 4800; 
};

template <> struct Traits<ATMega1281_Timer_1>: public Traits<ATMega1281_Common>
{
    static const int FREQUENCY = 720; // Hz
};

template <> struct Traits<ATMega1281_Timer_2>: public Traits<ATMega1281_Common>
{
    static const int FREQUENCY = 720; // Hz
};

template <> struct Traits<ATMega1281_Timer_3>: public Traits<ATMega1281_Common>
{
    static const int FREQUENCY = 720; // Hz
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

template <> struct Traits<ATMega1281_UART>: public Traits<void>
{
    enum {
        FULL    = 0, // Tx AND Rx enabled
    LIGHT   = 1, // Only Tx enabled
    STANDBY = 2, // Only Rx enabled
    OFF     = 3  // Tx AND Rx disabled
    };

    static const char Power_Management = FULL;
    static const unsigned int DEFAULT_UNIT = 1;
};

template <> struct Traits<ATMega1281_ADC>: public Traits<void>
{
};

template <> struct Traits<ATMega1281_SPI>: public Traits<ATMega1281_Common>
{
};

template <> struct Traits<ATMega1281_Display>: public Traits<ATMega1281_Common>
{
    static const bool on_serial = true;
};

template <> struct Traits<ATMega1281_NIC>: public Traits<void>
{
    typedef LIST<CMAC> NICS;

    static const unsigned int RADIO_UNITS = NICS::Count<Radio>::Result;
};

template <> struct Traits<CMAC>: public Traits<void>
{
    //static const bool enabled = false;
    static const unsigned int FREQUENCY = 0;
    static const unsigned int POWER     = 5;
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
