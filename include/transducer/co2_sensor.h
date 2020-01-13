// EPOS CO2 Concentration Smart Transducer Declarations

#ifndef __co2_sensor_h
#define __co2_sensor_h

#include <transducer.h>

#ifdef __mmod_emote3__

#include <machine/uart.h>
#include <machine/engine/cm1101.h>

__BEGIN_SYS

// Cubic Nondispersive Infrared (NDIR) Carbon Dioxide Single Beam Sensor CM1101
class CO2_Sensor: public Transducer<SmartData::Unit::I32 | SmartData::Unit::PPM>, private CM1101
{
private:
    static const unsigned int UART_BAUD_RATE = 9600;
    static const unsigned int UART_DATA_BITS = 8;
    static const unsigned int UART_PARITY = 0;
    static const unsigned int UART_STOP_BITS = UART_Common::NONE;

public:
    static const unsigned int DEVS = 1;

    static const int ERROR = 50; // PPM
    static const bool active = false;

public:
    CO2_Sensor(unsigned int dev = 0): CM1101(&_uart) {
        // _uart is being initialized after its being passed to CM1101, but the constructor is empty. Otherwise, use the comma operator
        assert(dev < DEVS);
    }

    Value sense() { return co2(); }

    template<typename SD>
    void sense(unsigned int dev, SD * sd) {
        sd->_value = co2();
    }

private:
    static UART _uart;
};

__END_SYS

#endif

#endif
