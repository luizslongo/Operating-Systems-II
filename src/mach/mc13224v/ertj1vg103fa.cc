// EPOS-- ERTJ1VG103FA Termistor Mediator Implementation

#include <mach/mc13224v/ertj1vg103fa.h>
#include <mach/mc13224v/battery.h>
#include <utility/math.h>

__USING_SYS

ERTJ1VG103FA::ERTJ1VG103FA(unsigned int unit)
 : _adc(ADC::SINGLE_ENDED_ADC0)
{
    _adc.get(); // discard first read
}

float ERTJ1VG103FA::convert_temperature(int value) {
    float res = resistance(value);
    if (res < 0) return 0;

    float logR = Math::logf(res);
    float T = (1 / (A + B * logR + C * logR * logR * logR)) + Correction;
    if (temperature_unit == CELCIUS)
        return T - Kelvin_to_Celsius;
    else
        return T;
}

int ERTJ1VG103FA::voltage(int read) {
    return (Battery::sys_batt().sample() * read / adc_max_value);
}

float ERTJ1VG103FA::resistance(int read) {
    return div_resistor * (((float) adc_max_value / (adc_max_value - read)) - 1.0);
}
