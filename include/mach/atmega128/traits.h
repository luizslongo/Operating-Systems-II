#ifndef __atmega128_traits_h
#define __atmega128_traits_h

__BEGIN_SYS

class ATMega128_Common;
template <> struct Traits<ATMega128_Common>: public Traits<void>
{
};

template <> struct Traits<ATMega128>: public Traits<ATMega128_Common>
{
    static const unsigned int CPUS = 1;
    //static const unsigned long long CLOCK = 7372800; //Mica2
    static const unsigned long long CLOCK = 8000000;
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;

    static const unsigned int APPLICATION_STACK_SIZE = 256;
    static const unsigned int APPLICATION_HEAP_SIZE = 512;

    static const unsigned int SYSTEM_STACK_SIZE = 64;
    static const unsigned int SYSTEM_HEAP_SIZE = 8 *  APPLICATION_STACK_SIZE;

};

template <> struct Traits<ATMega128_Timer_1>: public Traits<ATMega128_Common>
{
    // Should be between 30 and 7200 Hz
    static const int FREQUENCY = 720; // Hz

};

template <> struct Traits<ATMega128_Timer_2>: public Traits<ATMega128_Common>
{
    // Should be between 30 and 7200 Hz
    static const int FREQUENCY = 720; // Hz

};

template <> struct Traits<ATMega128_Timer_3>: public Traits<ATMega128_Common>
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
    static const bool enabled = false;

    typedef LIST<CMAC> NICS;

    static const unsigned int RADIO_UNITS = NICS::Count<Radio>::Result;
};

template <> struct Traits<CMAC>: public Traits<void>
{
    static const bool enabled = false;
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

__END_SYS

#endif
