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

public:
    MC13224V_ADC() {
		clock_divider(0x80);
		prescale(0x17);
		on_time(0xA);
		convert_time(0x14);
		mode(0x1);
		override(0x300);
		control(0x1);
	}

	MC13224V_ADC(unsigned char channel, Hertz frequency) {
	// TODO: frequency is not being set here! Needs fixing.
        clock_divider(0x80);
        prescale(0xef); // 100 KHz
        on_time(0xA);
        convert_time(0x28);
        mode(0x1);
        override(0x100 | channel);
        control(0x1);

        unsigned int x = 16800;
        while(x--); // will wait for stabilization
	}

	MC13224V_ADC(unsigned char channel, unsigned char reference,
		  unsigned char trigger, Hertz frequency) {
	}

	void config(unsigned char channel, unsigned char reference,
		unsigned char trigger, Hertz frequency) {
	}

	void config(unsigned char * channel, unsigned char * reference,
		unsigned char * trigger, Hertz * frequency) {
	}
    
	int sample() {
		return 0;
	}

	int get() {
		return (ad1_result());	
	}

	bool finished() {
		return false;
	}

	void reset() {
	}

	bool enable() { return true; }
	void disable() {}

private:
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

	static Reg16 ad1_result () { 
		return ARM7::in16(IO::ADC_AD1_RESULT); 
	}
	static void ad1_result(Reg16 value) {
		ARM7::out16(IO::ADC_AD1_RESULT, value); 
	}   

	static Reg16 ad2_result () { 
		return ARM7::in16(IO::ADC_AD2_RESULT); 
	}
	static void ad2_result(Reg16 value) {
		ARM7::out16(IO::ADC_AD2_RESULT, value); 
	}   

private:
	int _unit;
};

__END_SYS

#endif

