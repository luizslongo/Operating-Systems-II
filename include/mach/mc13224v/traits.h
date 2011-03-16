#ifndef __mc13224v_traits_h
#define __mc13224v_traits_h

#include <system/config.h>
#include <system/types.h>

// Mediators - Machine - MC13224V

__BEGIN_SYS

class MC13224V_Common;

template <> struct Traits<MC13224V_Common>: public Traits<void>
{
};

template <> struct Traits<MC13224V>: public Traits<MC13224V_Common>
{
	static const unsigned int MAX_CPUS = 1;
	static const unsigned int CLOCK = 24000000;
	static const unsigned int APPLICATION_STACK_SIZE = 1024;
	static const unsigned int APPLICATION_HEAP_SIZE = 3072;
	static const unsigned int SYSTEM_HEAP_SIZE = 2*APPLICATION_HEAP_SIZE;
	static const bool SMP = false;
};

template <> struct Traits<MC13224V_Battery>: public Traits<MC13224V_Common>
{
    static const bool enabled = true;
    static const bool buck_enabled = true;
    static const unsigned int buck_voltage_threshold = 2500; // in mV
    static const unsigned int frequency = 1; // in Hz
};

template <> struct Traits<MC13224V_Timer_0>: public Traits<MC13224V_Common>
{
    static const int FREQUENCY = 20;
};

template <> struct Traits<MC13224V_Timer_1>: public Traits<MC13224V_Common>
{
	static const int FREQUENCY = 10;//24000000/128.0;;
};

template <> struct Traits<MC13224V_Timer_2>: public Traits<MC13224V_Common>
{
	static const int FREQUENCY = 10;//24000000/128.0;
};

template <> struct Traits<MC13224V_Timer_3>: public Traits<MC13224V_Common>
{
	static const int FREQUENCY = 10;//24000000/128.0;
};

template <> struct Traits<MC13224V_RTC>: public Traits<MC13224V_Common>
{
	static const unsigned int EPOCH_DAYS = 1;
};

template <> struct Traits<MC13224V_Display>: public Traits<MC13224V_Common>
{
	static const bool on_serial = true;
};

template <> struct Traits<MC13224V_NIC>: public Traits<void>
{
    static const bool enabled = true;

    typedef LIST<MC13224V_Radio> NICS;

    static const unsigned int RADIO_UNITS = NICS::Count<MC13224V_Radio>::Result;
};

template <> struct Traits<CMAC<Radio_Wrapper> >: public Traits<void>
{
    static const bool debugged      = false;
    static const bool SM_STEP_DEBUG = false;

    static const unsigned char BROADCAST       = ~0;
    static const unsigned int  FREQUENCY       = 0;
    static const unsigned int  POWER           = 5;
    static const bool          TIME_TRIGGERED  = false;
    static const bool          COORDINATOR     = false;
    static const unsigned int  SLEEPING_PERIOD = 1000; //ms
    static const unsigned long TIMEOUT         = 50; //ms
    static const unsigned int  ADDRESS         = 0x0001;
    static const unsigned int  MTU             = 118; 

    typedef Generic_Sync<Radio_Wrapper>     Sync_State;
    typedef IEEE802154_Pack<Radio_Wrapper>  Pack_State;
    typedef Unslotted_CSMA_Contention<Radio_Wrapper>    Contention_State;
    typedef Generic_Tx<Radio_Wrapper>       Tx_State;
//    typedef IEEE802154_Ack_Rx<Radio_Wrapper>  Ack_Rx_State;
    typedef Ack_Rx_Empty<Radio_Wrapper>     Ack_Rx_State;
    typedef Generic_Lpl<Radio_Wrapper>      Lpl_State;
    typedef Generic_Rx<Radio_Wrapper>       Rx_State;
    typedef IEEE802154_Unpack<Radio_Wrapper>    Unpack_State;
//    typedef IEEE802154_Ack_Tx<Radio_Wrapper>  Ack_Tx_State;
    typedef Ack_Tx_Empty<Radio_Wrapper>     Ack_Tx_State;
};

__END_SYS

#endif

