// EPOS-- AVR8 TSC Mediator Declarations

#ifndef __avr8_tsc_h
#define __avr8_tsc_h

#include <tsc.h>

__BEGIN_SYS

class AVR8_TSC: public TSC_Common
{
private:
    // Imports
    typedef IO_Map<Machine> IO;

    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    
    // Timer1 Registers
    enum {
	TCCR1A = IO::TCCR1A,
	TCCR1B = IO::TCCR1B,
	TCNT1H = IO::TCNT1H,
	TCNT1L = IO::TCNT1L,
	OCR1AH = IO::OCR1AH,
	OCR1AL = IO::OCR1AL,
	OCR1BH = IO::OCR1BH,
	OCR1BL = IO::OCR1BL,
	ICR1H  = IO::ICR1H,
	ICR1L  = IO::ICR1L    
    };
    
    // Timer1 Register Bits
    enum {
    	// TCCR1A
    	COM1A1 = 0x80,
	COM1A0 = 0x40,
	COM1B1 = 0x20,
	COM1B0 = 0x10,
	FOC1A  = 0x08,
	FOC1B  = 0x04,
	WGM11  = 0x02,
	WGM10  = 0x01,
	// TCCR1B
	ICNC1  = 0x80,
	ICES1  = 0x40,
	WGM13  = 0x10,
	WGM12  = 0x08,
	CS12   = 0x04,
	CS11   = 0x02,
	CS10   = 0x01   
    };

public:
    AVR8_TSC() { /* Actual timer initialization is up to init */ }
    
    static Hertz frequency() { return Traits<Machine>::CLOCK / 8; }
    static Time_Stamp time_stamp() {  
	return tcnt1hl() | _ts << (sizeof(Reg16) * 8);
    }

    static int init(System_Info * si);
    
private:
    static Reg8 tccr1a() { return CPU::in8(TCCR1A); }
    static void tccr1a(Reg8 value) { CPU::out8(TCCR1A, value); }    
    static Reg8 tccr1b() { return CPU::in8(TCCR1B); }
    static void tccr1b(Reg8 value) { CPU::out8(TCCR1B, value); }       
    static Reg16 tcnt1hl() { return CPU::in16(TCNT1L); }
    static void tcnt1hl(Reg16 value) { return CPU::out16(TCNT1L, value); }

    static void timer_handler(unsigned int) { _ts++; } 

private:
    static volatile unsigned long _ts;
};

__END_SYS

#endif
