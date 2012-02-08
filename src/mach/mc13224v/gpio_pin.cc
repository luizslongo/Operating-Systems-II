// EPOS-- MC13224V Mediator Implementation

#include <mach/mc13224v/gpio_pin.h>

__BEGIN_SYS

volatile unsigned int MC13224V_GPIO_Pin::_data[2] = {0u, 0u};

MC13224V_GPIO_Pin::MC13224V_GPIO_Pin(int pin, bool dir)
  : _pin(pin)
{
    if(dir) output(_pin);
    else input(_pin);
}

void MC13224V_GPIO_Pin::input(int pin)
{
    int bit = pin % 32;
    int reg = (pin >> 5);

    // Set as input
    CPU::out32(IO::GPIO_PAD_DIR0 + (reg << 2),
                   CPU::in32(IO::GPIO_PAD_DIR0 + (reg << 2))
                    & ~(1 << bit)
    );
}

void MC13224V_GPIO_Pin::output(int pin)
{
    int bit = pin % 32;
    unsigned int reg = IO::GPIO_PAD_DIR0 + ((pin >> 5) << 2);

    // Set as output
    CPU::out32(reg, CPU::in32(reg) | (1 << bit));
}

void MC13224V_GPIO_Pin::set(int pin)
{
    int bit = pin % 32;
    int reg = (pin >> 5);

    // Set value
    _data[reg] |= (1 << bit);
    CPU::out32(IO::GPIO_DATA0 + (reg << 2), _data[reg]);
}

void MC13224V_GPIO_Pin::clear(int pin)
{
    int bit = pin % 32;
    int reg = (pin >> 5);

    // Set value
    _data[reg] &= ~(1 << bit);
    CPU::out32(IO::GPIO_DATA0 + (reg << 2), _data[reg]);
}

bool MC13224V_GPIO_Pin::get(int pin)
{
    int bit = pin % 32;
    int reg = IO::GPIO_DATA0 + ((pin >> 5) << 2);

    // Get value
    return ((1 << bit) & CPU::in32(reg));
}

__END_SYS
