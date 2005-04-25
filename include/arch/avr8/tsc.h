// EPOS-- AVR8_TSC Declarations

#ifndef __avr8_tsc_h
#define __avr8_tsc_h

#include <tsc.h>
#include <mach/avrmcu/ic.h>
#include <mach/avrmcu/avrmcu.h>

__BEGIN_SYS

class AVR8_TSC: public TSC_Common
{
private:
    typedef Traits<AVR8_TSC> Traits;
    static const Type_Id TYPE = Type<AVR8_TSC>::TYPE;
    typedef unsigned char Reg8;
    typedef unsigned int Reg16;
    static Time_Stamp _ts;
    
    // Timer1 Registers
    enum {
	SFIOR = 0x30,
	TCCR1A = 0x2f,
	TCCR1B = 0x2e,
	TCNT1H = 0x2d,
	TCNT1L = 0x2c,
	OCR1AH = 0x2b,
	OCR1AL = 0x2a,
	OCR1BH = 0x29,
	OCR1BL = 0x28,
	ICR1H = 0x27,
	ICR1L = 0x26    
    };
    
    // Timer1 Register Bits
    enum {
    	// TCCR1A
    	COM1A1 = 0x80,
	COM1A0 = 0x40,
	COM1B1 = 0x20,
	COM1B0 = 0x10,
	FOC1A = 0x08,
	FOC1B = 0x04,
	WGM11 = 0x02,
	WGM10 = 0x01,
	// TCCR1B
	ICNC1 = 0x80,
	ICES1 = 0x40,
	WGM13 = 0x10,
	WGM12 = 0x08,
	CS12 = 0x04,
	CS11 = 0x02,
	CS10 = 0x01   
    };

public:
    AVR8_TSC(){ /* Actual timer initialization is up to init */ };
    ~AVR8_TSC(){};
    
    static Hertz frequency(){ return Traits<AVR8>::CLOCK/8; };
    static Time_Stamp time_stamp(){ return tcnt1hl() /*(_ts | tcnt1hl())*/; };

    static int init(System_Info *si);
    
private:

    static Reg8 tccr1a(){
      return AVR8::in8(TCCR1A);
    }
    
    static void tccr1a(Reg8 value){
      AVR8::out8(TCCR1A,value);
    }    
    
    static Reg8 tccr1b(){
      return AVR8::in8(TCCR1B);
    }
    
    static void tccr1b(Reg8 value){
      AVR8::out8(TCCR1B,value);
    }       
    
    static Reg16 tcnt1hl(){
      return AVR8::in16(TCNT1L);
    }
    
    static void tcnt1hl(Reg16 value){
      return AVR8::out16(TCNT1L,value);
    }      
    
    static void timer1_handler(void)
    {
    	//_ts += 0x10000;
    } 
};

__END_SYS

#endif
