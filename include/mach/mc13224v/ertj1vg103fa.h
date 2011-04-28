// EPOS ERTJ1VG103FA Sensor Mediator Declarations

#ifndef __ertj1vg103fa_h
#define __ertj1vg103fa_h

#include <adc.h>

__BEGIN_SYS

class ERTJ1VG103FA {
protected:
    static const int div_resistor = 10000; // 10 kohm
    static const int adc_max_value = 2047; // 11 bits ADC
    static const int temperature_unit = Traits<Temperature_Sensor>::UNITS;

    enum {
        KELVIN = 0,
        CELCIUS
    };

    static const float A = 0.0010750492;
    static const float B = 0.00027028218;
    static const float C = 0.00000014524838;
    static const float Correction = 4.37;
    static const float Kelvin_to_Celsius = 273.15;

public:
    ERTJ1VG103FA(unsigned int unit = 0);
    ~ERTJ1VG103FA() {}

    int get() { return resistance(_adc.get()); }

    int sample() { return convert_temperature(_adc.get()); }

    bool enable() { return true;}
    void disable() {}
    bool data_ready() { return _adc.finished(); }

private:
    float convert_temperature(int value);
    int voltage(int read);
    float resistance(int read);

    ADC _adc;
};

__END_SYS

#endif

