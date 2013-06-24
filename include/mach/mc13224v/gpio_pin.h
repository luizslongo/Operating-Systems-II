// EPOS-- GPIO Pin Mediator Declarations

#ifndef __mc13224v_gpio_pin_h
#define __mc13224v_gpio_pin_h

#include <gpio_pin.h>
#include <machine.h>

__BEGIN_SYS

class MC13224V_GPIO_Pin: public GPIO_Pin_Common {
private:
    MC13224V_GPIO_Pin();

    typedef Machine::IO IO;

public:
    MC13224V_GPIO_Pin(int pin);
    ~MC13224V_GPIO_Pin() {}

    void put(bool value) { value ? set(_pin) : clear(_pin); }
    void set() { set(_pin); }
    void clear() { clear(_pin); }
    bool get() { return get(_pin); }

private:
    static void input(int pin);
    static void output(int pin);
    static void function(int pin);
    static void set(int pin);
    static void clear(int pin);
    static bool get(int pin);

    int _pin;
};

__END_SYS

#endif
