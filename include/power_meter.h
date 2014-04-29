// EPOS Power Meter Mediator Declarations

#ifndef __power_meter_h
#define __power_meter_h

#include <adc.h>
#include <alarm.h>

__BEGIN_SYS

class Power_Meter {
private:
    static const int DEFAULT_FREQ = 2000;
    static const int n_samples = 128;

public:
    Power_Meter(int v_channel = ADC::SINGLE_ENDED_ADC0,
                int acs712_channel = ADC::SINGLE_ENDED_ADC1,
                float f_s = DEFAULT_FREQ)
    : _v_adc(v_channel, ADC::SYSTEM_REF, ADC::SINGLE_CONVERSION_MODE, Traits<Machine>::CLOCK>>9, 0),
      _acs712(acs712_channel, 1, Traits<Machine>::CLOCK>>9),
      _f_s(f_s) {}

    ~Power_Meter() {}

    void start() {}
    void stop() {}
    float get() {
        int _i;
        int _v_samples[n_samples];
        int _v_shift[n_samples];
        int _i_samples[n_samples];
        long _i_mean;
        long _p_mean;
        long _p_samples[n_samples];

        _i_mean = 0;

        for(_i = 0; _i < n_samples; _i++) {
            _i_samples[_i] = _acs712.get();
            _v_samples[_i] = _v_adc.get();

            _v_shift[(_i+_f_s/(2*60))%n_samples] = _v_samples[_i];

            _i_mean += _i_samples[_i];

            Alarm::delay(1000000/_f_s);
        }

        _i_mean = _i_mean/n_samples;

        for(_i = 0; _i < n_samples; _i++) {
            _v_samples[_i] = _v_samples[_i]-_v_shift[_i];
            _i_samples[_i] = _i_samples[_i]-_i_mean;
            _p_samples[_i] = _v_samples[_i]*_i_samples[_i];
        }

        _p_mean = 0;

        for(_i = 19; _i < n_samples; _i++) {
            _p_mean += _p_samples[_i];
        }

        _p_mean = _p_mean/(n_samples-20);

        return (0.00082360*_p_mean);
    }

private:
    int _f_s;
    ADC _v_adc;
    ACS712 _acs712;
};

__END_SYS

#endif
