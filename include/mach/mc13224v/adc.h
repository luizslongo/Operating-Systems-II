// EPOS-- MC13224V ADC Mediator Declarations

#ifndef __mc13224v_adc_h
#define __mc13224v_adc_h

#include <system/memory_map.h>
#include <tsc.h>
#include <cpu.h>
#include <adc.h>

__BEGIN_SYS

class MC13224V_ADC: public ADC_Common
{
private:
    typedef IO_Map<Machine> IO;
    typedef TSC::Hertz Hertz;
    typedef ARM7::Reg8 Reg8;
    typedef ARM7::Reg16 Reg16;
    typedef ARM7::Reg32 Reg32;
    typedef Traits<ADC> _Traits;

    static const Hertz DEFAULT_FREQUENCY = Traits<Machine>::CLOCK>>9;

    enum {
        CONTROL_MASTER_ON = 0x1,
        MODE_OVERRIDE     = 0x1,
    };

public:
    MC13224V_ADC(unsigned char channel = SINGLE_ENDED_ADC0,
                 unsigned char reference = SYSTEM_REF,
                 unsigned char trigger = SINGLE_CONVERSION_MODE,
                 Hertz frequency = DEFAULT_FREQUENCY, int unit = 0)
    : _unit(unit), _shift((unit+1)%2), _override_adc_unit_on(0x100 << _shift),
      _override_adc_other_unit_on(0x300 >> _shift) {
        config(channel, reference, trigger, frequency);
    }

    void config(unsigned char channel, unsigned char reference,
                unsigned char trigger, Hertz frequency) {
        // Must aways produce < or = 300000Hz
        clock_divider(calculate_clock_divider(300000));
        // Usually results 1000000Hz
        prescale(calculate_prescale(1000000));
        // ON TIME must be >= 10us
        on_time(calculate_on_time(10));
        // CONVERT TIME must be >= 20us, depends on prescale aready configured
        convert_time(calculate_convert_time((double)1000000/(frequency)));

        // Setting the ADC to manual mode
        mode(MODE_OVERRIDE);

        // Turning this unit on, setting this unit channel and
        // turning the master device on
        turn_unit_on();
        set_unit_channel(channel);
        turn_master_on();

        // Will wait for stabilization. I don't know if it's
        // really necessary, but we should wait >= ON TIME.
        unsigned int x = 16800;
        while(x--);
    }

    void config(unsigned char * channel, unsigned char * reference,
                unsigned char * trigger, Hertz * frequency) {}

    int sample() { return 0; }
    int get(){ return ad_result(); }
    bool finished() { return false; }
    void reset() {}
    bool enable() { return true; }
    void disable() {}

    bool power(_Traits::Power_Modes mode) {
        switch(mode){
            case _Traits::OFF:
                override(override() & ~(_override_adc_unit_on));
                if(!(override() & _override_adc_other_unit_on)){
                    control(control() & ~CONTROL_MASTER_ON);
                }
                return true;
            break;
            case _Traits::LIGHT:
            break;
            case _Traits::STANDBY:
            break;
            case _Traits::FULL:
                control(CONTROL_MASTER_ON);
                override(override() | _override_adc_unit_on);
                //setup
                return true;
            break;
        }

        return false;
    }

    _Traits::Power_Modes power(){
        if(override() & _override_adc_unit_on){
            return _Traits::FULL;
        }
        return _Traits::OFF;
    }

private:
    unsigned short calculate_clock_divider(Hertz adc_clock){
        return ((double)Traits<Machine>::CLOCK/adc_clock);
    }

    unsigned short calculate_prescale(Hertz adc_prescale_clock){
        return ((double)Traits<Machine>::CLOCK/adc_prescale_clock) - 1;
    }

    unsigned short calculate_on_time(unsigned int on_time){
        return ((double)1/(Traits<Machine>::CLOCK/(prescale()+1)))*on_time*1000000;
    }

    unsigned short calculate_convert_time(unsigned int convert_time){
        return ((double)1/(Traits<Machine>::CLOCK/(prescale()+1)))*convert_time*1000000;
    }

    void turn_unit_on() {
        override(override() | _override_adc_unit_on);
    }

    void set_unit_channel(unsigned char channel) {
        override((override() & ~(0xF << _shift*4)) | (channel << _shift*4)); // clear and set
    }

    void turn_master_on(){
        // (control() | CONTRO_MASTER_ON) was breaking the samples
        control(CONTROL_MASTER_ON);
    }

    static Reg16 clock_divider () {
        return ARM7::in16(IO::ADC_CLOCK_DIVIDER);
    }
    static void clock_divider(Reg16 value) {
        ARM7::out16(IO::ADC_CLOCK_DIVIDER, value);
    }

    static Reg16 prescale () {
        return ARM7::in16(IO::ADC_PRESCALE);
    }
    static void prescale(Reg16 value) {
        ARM7::out16(IO::ADC_PRESCALE, value);
    }

    static Reg16 on_time () {
        return ARM7::in16(IO::ADC_ON_TIME);
    }
    static void on_time(Reg16 value) {
        ARM7::out16(IO::ADC_ON_TIME, value);
    }

    static Reg16 convert_time () {
        return ARM7::in16(IO::ADC_CONVERT_TIME);
    }
    static void convert_time(Reg16 value) {
        ARM7::out16(IO::ADC_CONVERT_TIME, value);
    }

    static Reg16 mode () {
        return ARM7::in16(IO::ADC_MODE);
    }
    static void mode(Reg16 value) {
        ARM7::out16(IO::ADC_MODE, value);
    }

    static Reg16 override () {
        return ARM7::in16(IO::ADC_OVERRIDE);
    }
    static void override(Reg16 value) {
        ARM7::out16(IO::ADC_OVERRIDE, value);
    }

    static Reg16 control () {
        return ARM7::in16(IO::ADC_CONTROL);
    }
    static void control(Reg16 value) {
        ARM7::out16(IO::ADC_CONTROL, value);
    }

    Reg16 ad_result () {
        if(_unit == 1)
            return ARM7::in16(IO::ADC_AD1_RESULT);
        else
            return ARM7::in16(IO::ADC_AD2_RESULT);
    }
    void ad_result(Reg16 value) {
        if(_unit == 1)
            ARM7::out16(IO::ADC_AD1_RESULT, value);
        else
            ARM7::out16(IO::ADC_AD2_RESULT, value);
    }

private:
    int _unit;
    int _shift;
    int _override_adc_unit_on;
    int _override_adc_other_unit_on;
};

__END_SYS

#endif
