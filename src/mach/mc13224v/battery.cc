// EPOS-- MC13224V Buck Regulator Mediator Implementation

#include <mach/mc13224v/battery.h>
#include <mach/mc13224v/buck_regulator.h>
#include <mach/mc13224v/adc.h>

__BEGIN_SYS

MC13224V_Battery * MC13224V_Battery::system_battery;

__END_SYS

__USING_SYS

MC13224V_Battery::MC13224V_Battery()
 : _adc(1, 1000000)
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::MC13224V_Battery()\n";
}

const unsigned short MC13224V_Battery::get()
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::get()\n";

    return 3000;//_adc.get();
}

const unsigned short MC13224V_Battery::sample()
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::sample()\n";

    return read_to_voltage(get());
}

const unsigned short MC13224V_Battery::charge()
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::charge()\n";

    return read_to_charge(get());
}

MC13224V_Battery & MC13224V_Battery::sys_batt()
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::sys_batt()\n";

    return *system_battery;
}
const unsigned short MC13224V_Battery::read_to_voltage(unsigned short value)
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::read_to_voltage(" << value << ")\n";

    if(value <= 0u) return 0;

    return (battery_reference * adc_max) / value;
}

const unsigned short MC13224V_Battery::read_to_charge(unsigned short value)
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::read_to_charge(" << value << ")\n";

    //"Dummy" linear model
    return 100 - (100 * (value - ADC_3p3v) / (ADC_1p8v - ADC_3p3v));
}

void MC13224V_Battery::check_buck()
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::check_buck()\n";

    check_buck(get());
}

void MC13224V_Battery::check_buck(unsigned short read)
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::check_buck(" << read << ")\n";

    if((read < buck_disable_adc_threshold) && (!MC13224V_Buck_Regulator::is_in_bypass()))
    {
        MC13224V_Buck_Regulator::enter_bypass();
        disable_battery_under_threshold();
        enable_battery_over_threshold();
    }
    else if((read > buck_enable_adc_threshold) && (MC13224V_Buck_Regulator::is_in_bypass()))
    {
        MC13224V_Buck_Regulator::leave_bypass();
        disable_battery_over_threshold();
        enable_battery_under_threshold();
    }
}

const void MC13224V_Battery::enable_battery_over_threshold()
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::enable_battery_over_threshold()\n";

    // set 0x0200 in ?
}

const void MC13224V_Battery::disable_battery_over_threshold()
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::disable_battery_over_threshold()\n";

    // clear 0x0200 in ?
}

const void MC13224V_Battery::enable_battery_under_threshold()
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::enable_battery_under_threshold()\n";

    // set 0x0100 in ?
}

const void MC13224V_Battery::disable_battery_under_threshold()
{
    db<MC13224V_Battery>(TRC) << "MC13224V_Battery::disable_battery_under_threshold()\n";

    // clear 0x0100 in ?
}
