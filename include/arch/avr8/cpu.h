// EPOS-- AVR8 Declarations 

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.

#ifndef __avr8_h
#define __avr8_h

#include <cpu.h>
#include <utility/debug.h>
#include <system/info.h>

__BEGIN_SYS

class AVR8: public CPU_Common
{
private:
    typedef Traits<AVR8> Traits;
    static const Type_Id TYPE = Type<AVR8>::TYPE;

    // AVR8 private imports, types and constants

public:
    // Memory page
    typedef unsigned char Page;
    
    
    // CPU Flags
    typedef Reg8 Flags;
    enum {  
      FLAG_CARRY        = 0x01,
      FLAG_ZERO         = 0x02,
      FLAG_NEGATIVE     = 0x04,
      FLAG_TWO_COMPL_OF = 0x08,
      FLAG_SIGN         = 0x10,   
      FLAG_HALFCARRY    = 0x20,
      FLAG_BIT_COPY_ST  = 0x40,
      FLAG_INTERRUPT    = 0x80,
      FLAG_DEFAULTS     = FLAG_INTERRUPT, 
      FLAG_CLEAR        = ~FLAG_DEFAULTS
    };


    class Context {
		
    public:
      Context(Log_Addr entry) :_sreg(FLAG_DEFAULTS)/*, _pc(entry)*/ {}   
	
	Context() {}

	void save() volatile;
	void load() const volatile;

	friend Debug & operator << (Debug & db, const Context & c) {
	  db << "{"
	     << "sp="  << &c
	     << ",sreg=" << c._sreg
	     << "}" ;
	  return db;
	}

    private:
	Reg8 _sreg;
	Reg8 _r0;
	//Reg8 _r1;		// r1 is always 0
	Reg8 _r2;
	Reg8 _r3;
	Reg8 _r4;
	Reg8 _r5;
	Reg8 _r6;
	Reg8 _r7;
	Reg8 _r8;
	Reg8 _r9;
	Reg8 _r10;
	Reg8 _r11;
	Reg8 _r12;
	Reg8 _r13;
	Reg8 _r14;
	Reg8 _r15;
	Reg8 _r16;
	Reg8 _r17;
	Reg8 _r18;
	Reg8 _r19;
	Reg8 _r20;
	Reg8 _r21;
	Reg8 _r22;
	Reg8 _r23;
	//Reg8 _r24;
	//Reg8 _r25;
	//Reg8 _r26;
	//Reg8 _r27;
	Reg8 _r28;
	Reg8 _r29;
	Reg8 _r30;
	Reg8 _r31;
	Reg16 _pc;		
    };
    
    class IO_Registers {
    public:
      typedef unsigned volatile char IOReg8;
    
      IOReg8 twbr;    	// [0x20]
      IOReg8 twsr;    	// [0x21]
      IOReg8 twar;    	// [0x22]
      IOReg8 twdr;    	// [0x23]
      IOReg8 adcl;    	// [0x24]
      IOReg8 adch;    	// [0x25]
      IOReg8 adcsra;    // [0x26]
      IOReg8 admux;    	// [0x27]
      IOReg8 acsr;    	// [0x28]   
      IOReg8 ubrrl;    	// [0x29]
      IOReg8 ucsrb;    	// [0x2A]
      IOReg8 ucsra;    	// [0x2B]
      IOReg8 udr;    	// [0x2C]
      IOReg8 spcr;    	// [0x2D]
      IOReg8 spsr;    	// [0x2E]
      IOReg8 spdr;    	// [0x2F]
      IOReg8 pind;    	// [0x30]
      IOReg8 ddrd;    	// [0x31]
      IOReg8 portd;    	// [0x32]
      IOReg8 pinc;    	// [0x33]
      IOReg8 ddrc;    	// [0x34]
      IOReg8 portc;    	// [0x35]
      IOReg8 pinb;    	// [0x36]
      IOReg8 ddrb;    	// [0x37]
      IOReg8 portb;    	// [0x38]
      IOReg8 pina;    	// [0x39] 
      IOReg8 ddra;    	// [0x3A]
      IOReg8 porta;    	// [0x3B] 
      IOReg8 eecr;    	// [0x3C] 
      IOReg8 eedr;    	// [0x3D]
      IOReg8 eearl;    	// [0x3E]
      IOReg8 eearh;    	// [0x3F]
      union
      {
	IOReg8 ubrrh;	// [0x40]
	IOReg8 ucsrc;
      };
      IOReg8 wdtcr;    	// [0x41]
      IOReg8 assr;    	// [0x42]
      IOReg8 ocr2;    	// [0x43]
      IOReg8 tcnt2;    	// [0x44]
      IOReg8 tccr2;    	// [0x45]
      IOReg8 icr1l;    	// [0x46]
      IOReg8 icr1h;    	// [0x47]
      IOReg8 ocr1bl;    // [0x48]
      IOReg8 ocr1bh;    // [0x49]
      IOReg8 ocr1al;    // [0x4A]
      IOReg8 ocr1ah;    // [0x4B]
      IOReg8 tcnt1l;    // [0x4C]
      IOReg8 tcnt1h;    // [0x4D]
      IOReg8 tccr1b;    // [0x4E]
      IOReg8 tccr1a;    // [0x4F]
      IOReg8 sfior;    	// [0x50]
      union
      {
	IOReg8 osccal;	// [0x51]
	IOReg8 ocdr;
      };
      IOReg8 tcnt0;    	// [0x52]
      IOReg8 tccr0;    	// [0x53]
      IOReg8 mcucsr;    // [0x54]
      IOReg8 mcucr;    	// [0x55]
      IOReg8 twcr;    	// [0x56]
      IOReg8 spmcr;    	// [0x57]
      IOReg8 tifr;    	// [0x58]
      IOReg8 timsk;    	// [0x59]
      IOReg8 gifr;    	// [0x5A]
      IOReg8 gicr;     	// [0x5B]
      IOReg8 ocr0;    	// [0x5C] 
      IOReg8 spl;    	// [0x5D]
      IOReg8 sph;    	// [0x5E]
      IOReg8 sreg;    	// [0x5F]
    };
	

public:
    Hertz clock(){ return 0; };

    static void int_enable(){ASMV("SEI");};
    static void int_disable(){ASMV("CLI");};
    static void halt(){ASMV("SLEEP");};

    static void switch_context(Context * volatile * current, 
			       Context * volatile next);

    static bool tsl(volatile bool & lock){ return false;};
    static int finc(volatile int & number){ return 0;};
    static int fdec(volatile int & number){ return 0;};

    static int init(System_Info * si);

    static Reg16 fr() { return 0; }

    // AVR8 implementation methods
    
    static unsigned char in8(const unsigned char port) {
	return (*(volatile unsigned char *)(port + 0x20));
    }
    
    static void out8(unsigned char port, unsigned char value) {
	(*(volatile unsigned char *)(port + 0x20)) = value;
    }

    static volatile IO_Registers * io(){
	return (AVR8::IO_Registers *)(0x20);
    }
    
private:
    // AVR8 attributes
};

__END_SYS

#endif
