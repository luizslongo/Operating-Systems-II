// EPOS-- ATMega128 Interrupt Controller Mediator Declarations

#ifndef __atmega128_ic_h
#define __atmega128_ic_h

#include <system/memory_map.h>
#include <cpu.h>
#include "../avr_common/ic.h"

__BEGIN_SYS

class ATMega128_IC: public IC_Common, private AVR_IC
{
private:
    typedef IO_Map<Machine> IO;

    // Interrupt IO Registers
    enum {	
	EIMSK   = IO::EIMSK,   // Manual Page 89
	EIFR    = IO::EIFR,
	TIMSK   = IO::TIMSK,   // Manual Page 138
	TIFR    = IO::TIFR,
	ETIMSK  = IO::ETIMSK,
	ETIFR   = IO::ETIFR,
	SPCR    = IO::SPCR,    // Manual Page 166
	SPSR    = IO::SPSR,
	UCSR0A  = IO::UCSR0A,  // Manual Page 189
	UCSR0B  = IO::UCSR0B,
	UCSR1A  = IO::UCSR1A,
	UCSR1B  = IO::UCSR1B,
	ADCSRA  = IO::ADCSRA,  // Manual Page 245
	EECR    = IO::EECR,    // Manual Page 20
	ACSR    = IO::ACSR     // Manual Page 228
    };    
    
    // Interrupt Register Flags
    enum {
	// EIMSK
	INT7     = 0x80,
	INT6     = 0x40,
	INT5     = 0x20,
	INT4     = 0x10,
	INT3     = 0x08,
	INT2     = 0x04,
	INT1     = 0x02,
	INT0     = 0x01,
	// EIFR
	INTF7    = 0x80,
	INTF6    = 0x40,
	INTF5    = 0x20,
	INTF4    = 0x10,
	INTF3    = 0x08,
	INTF2    = 0x04,
	INTF1    = 0x02,
	INTF0    = 0x01,
	// TIMSK
	OCIE2	 = 0x80,
	TOIE2	 = 0x40,
	TICIE1	 = 0x20,
	OCIE1A	 = 0x10,
	OCIE1B	 = 0x08,
	TOIE1	 = 0x04,
	OCIE0	 = 0x02,
	TOIE0	 = 0x01,
	// TIFR
	OCF2	 = 0x80,
	TOV2	 = 0x40,
	ICF1	 = 0x20,
	OCF1A	 = 0x10,
	OCF1B	 = 0x08,
	TOV1	 = 0x04,
	OCF0	 = 0x02,
	TOV0	 = 0x01,
	// ETIMSK
	TICIE3	 = 0x20,
	OCIE3A	 = 0x10,
	OCIE3B	 = 0x08,
	TOIE3	 = 0x04,
	OCIE3C	 = 0x02,
	OCIE1C	 = 0x01,
	// ETIFR
	ICF3	 = 0x20,
	OCF3A	 = 0x10,
	OCF3B	 = 0x08,
	TOV3	 = 0x04,
	OCF3C	 = 0x02,
	OCF1C	 = 0x01,
	// TWCR
	TWINT	 = 0x80,
	TWIE	 = 0x01,
	// SPCR
	SPIE	 = 0x80,
	// SPSR
	SPIF	 = 0x80,
	// UCSRnA
	RXCn	 = 0x80,
	TXCn	 = 0x40,
	UDREn	 = 0x20,
	// UCSRnB
	RXCIEn	 = 0x80,
	TXCIEn	 = 0x40,
	UDRIEn	 = 0x20,
	//ADCSRA
	ADIF	 = 0x10,
	ADIE	 = 0x80,
	// EECR
	EERIE	 = 0x08,
	// ACSR
	ACI	 = 0x10,
	ACIE	 = 0x80
    };

public:
    // IRQs
    enum {
	IRQ0         = 0x00000002,
	IRQ1         = 0x00000004,
	IRQ2         = 0x00000008,
	IRQ3         = 0x00000010,
	IRQ4         = 0x00000020,
	IRQ5         = 0x00000040,
	IRQ6         = 0x00000080,
	IRQ7         = 0x00000100,
	TIMER2_COMP  = 0x00000200,
	TIMER2_OVF   = 0x00000400,
	TIMER1_CAPT  = 0x00000800,
	TIMER1_COMPA = 0x00001000,
	TIMER1_COMPB = 0x00002000,
	TIMER1_OVF   = 0x00004000,
	TIMER0_COMP  = 0x00008000,
	TIMER0_OVF   = 0x00010000,
	SPI_STC      = 0x00020000,
	USART0_RXC   = 0x00040000,
	USART0_UDRE  = 0x00080000,
	USART0_TXC   = 0x00100000,
	ADC          = 0x00200000,
	EE_RDY       = 0x00400000,
	ANA_COMP     = 0x00800000,
	TIMER1_COMPC = 0x01000000,
	TIMER3_CAPT  = 0x02000000,
	TIMER3_COMPA = 0x04000000,
	TIMER3_COMPB = 0x08000000,
	TIMER3_COMPC = 0x10000000,
	TIMER3_OVF   = 0x20000000,
	USART1_RXC   = 0x40000000,
	USART1_UDRE  = 0x80000000,
	USART1_TXC   = 0x00000001,
    };

    enum {
	IRQ_RESET        = 0,
	IRQ_IRQ0         = 1,
	IRQ_IRQ1         = 2,
	IRQ_IRQ2         = 3,
	IRQ_IRQ3         = 4,
	IRQ_IRQ4         = 5,
	IRQ_IRQ5         = 6,
	IRQ_IRQ6         = 7,
	IRQ_IRQ7         = 8,
	IRQ_TIMER2_COMP  = 9,
	IRQ_TIMER2_OVF   = 10,
	IRQ_TIMER1_CAPT  = 11,
	IRQ_TIMER1_COMPA = 12,
	IRQ_TIMER1_COMPB = 13,
	IRQ_TIMER1_OVF   = 14,
	IRQ_TIMER0_COMP  = 15,
	IRQ_TIMER0_OVF   = 16,
	IRQ_SPI_STC      = 17,
	IRQ_USART0_RXC   = 18,
	IRQ_USART0_UDRE  = 19,
	IRQ_USART0_TXC   = 20,
	IRQ_ADC          = 21,
	IRQ_EE_RDY       = 22,
	IRQ_ANA_COMP     = 23,
	IRQ_TIMER1_COMPC = 24,
	IRQ_TIMER3_CAPT  = 25,
	IRQ_TIMER3_COMPA = 26,
	IRQ_TIMER3_COMPB = 27,
	IRQ_TIMER3_COMPC = 28,
	IRQ_TIMER3_OVF   = 29,
	IRQ_USART1_RXC   = 30,
	IRQ_USART1_UDRE  = 31,
	IRQ_USART1_TXC   = 32,
	IRQ_TWI          = 33,
	IRQ_SPM_RDY      = 34,
    	IRQ_TSC 	 = IRQ_TIMER1_OVF,	
	IRQ_TIMER 	 = IRQ_TIMER0_COMP
    };

public:
    ATMega128_IC() {};
    
    static void enable(IRQ irq) {
        if (IRQ_IRQ0  	 == irq) { eimsk(eimsk() | INT0); return; }
        if (IRQ_IRQ1  	 == irq) { eimsk(eimsk() | INT1); return; }
        if (IRQ_IRQ2   	 == irq) { eimsk(eimsk() | INT2); return; }
        if (IRQ_IRQ3   	 == irq) { eimsk(eimsk() | INT3); return; }
        if (IRQ_IRQ4   	 == irq) { eimsk(eimsk() | INT4); return; }
        if (IRQ_IRQ5   	 == irq) { eimsk(eimsk() | INT5); return; }
        if (IRQ_IRQ6   	 == irq) { eimsk(eimsk() | INT6); return; }
        if (IRQ_IRQ7   	 == irq) { eimsk(eimsk() | INT7); return; }
	
        if (IRQ_TIMER2_COMP  == irq) { timsk(timsk() | OCIE2); return; }
        if (IRQ_TIMER2_OVF   == irq) { timsk(timsk() | TOIE2); return; }
	
        if (IRQ_TIMER1_CAPT  == irq) { timsk(timsk() | TICIE1); return; } 
        if (IRQ_TIMER1_COMPA == irq) { timsk(timsk() | OCIE1A); return; }  
        if (IRQ_TIMER1_COMPB == irq) { timsk(timsk() | OCIE1B); return; } 
        if (IRQ_TIMER1_OVF   == irq) { timsk(timsk() | TOIE1); return; } 
	if (IRQ_TIMER1_COMPC == irq) { etimsk(etimsk() | OCIE1C); return; }

        if (IRQ_TIMER0_OVF   == irq) { timsk(timsk() | TOIE0); return; } 
        if (IRQ_TIMER0_COMP  == irq) { timsk(timsk() | OCIE0); return; } 
	
        if (IRQ_SPI_STC      == irq) { spcr(spcr() | SPIE); return; }
	
        if (IRQ_USART0_RXC   == irq) { ucsr0b(ucsr0b() | RXCIEn); return; } 
        if (IRQ_USART0_UDRE  == irq) { ucsr0b(ucsr0b() | UDRIEn); return; } 
        if (IRQ_USART0_TXC   == irq) { ucsr0b(ucsr0b() | TXCIEn); return; } 

        if (IRQ_USART1_RXC   == irq) { ucsr1b(ucsr1b() | RXCIEn); return; } 
        if (IRQ_USART1_UDRE  == irq) { ucsr1b(ucsr1b() | UDRIEn); return; } 
        if (IRQ_USART1_TXC   == irq) { ucsr1b(ucsr1b() | TXCIEn); return; }  

        if (IRQ_ADC  	     == irq) { adcsra(adcsra() | ADIE); return; }
	
        if (IRQ_ANA_COMP     == irq) { acsr(acsr() | ACIE); return; }
	
        if (IRQ_EE_RDY       == irq) { eecr(eecr() | EERIE); return; }

        if (IRQ_TIMER3_CAPT  == irq) { etimsk(etimsk() | TICIE3); return; } 
        if (IRQ_TIMER3_COMPA == irq) { etimsk(etimsk() | OCIE3A); return; }  
        if (IRQ_TIMER3_COMPB == irq) { etimsk(etimsk() | OCIE3B); return; } 
        if (IRQ_TIMER3_OVF   == irq) { etimsk(etimsk() | TOIE3); return; } 
	if (IRQ_TIMER3_COMPC == irq) { etimsk(etimsk() | OCIE3C); return; }
    }
    
    static void disable() {
        eimsk(eimsk() & ~(INT0 | INT1 | INT2 | INT3 |
			  INT4 | INT5 | INT6 | INT7));
	
	timsk(timsk() & ~(OCIE2 | TOIE2 | TICIE1 | OCIE1A 
			  | OCIE1B | TOIE1 | TOIE0 | OCIE0)); 

	etimsk(etimsk() & ~OCIE1C);
	
        spcr(spcr() & ~SPIE);
	
        ucsr0b(ucsr0b() & ~(RXCIEn | UDRIEn | TXCIEn)); 

        ucsr1b(ucsr1b() & ~(RXCIEn | UDRIEn | TXCIEn));
	
        adcsra(adcsra() & ~ADIE);
	
        acsr(acsr() & ~ACIE);
	
        eecr(eecr() & ~EERIE);

        etimsk(etimsk() & ~TICIE3); 
        etimsk(etimsk() & ~OCIE3A);  
        etimsk(etimsk() & ~OCIE3B); 
        etimsk(etimsk() & ~TOIE3); 
	etimsk(etimsk() & ~OCIE3C);
    }
    static void disable(IRQ irq) {
        if (IRQ_IRQ0  	 == irq) { eimsk(eimsk() & ~INT0); return; }
        if (IRQ_IRQ1  	 == irq) { eimsk(eimsk() & ~INT1); return; }
        if (IRQ_IRQ2   	 == irq) { eimsk(eimsk() & ~INT2); return; }
        if (IRQ_IRQ3   	 == irq) { eimsk(eimsk() & ~INT3); return; }
        if (IRQ_IRQ4   	 == irq) { eimsk(eimsk() & ~INT4); return; }
        if (IRQ_IRQ5   	 == irq) { eimsk(eimsk() & ~INT5); return; }
        if (IRQ_IRQ6   	 == irq) { eimsk(eimsk() & ~INT6); return; }
        if (IRQ_IRQ7   	 == irq) { eimsk(eimsk() & ~INT7); return; }
	
        if (IRQ_TIMER2_COMP  == irq) { timsk(timsk() & ~OCIE2); return; }
        if (IRQ_TIMER2_OVF   == irq) { timsk(timsk() & ~TOIE2); return; }
	
        if (IRQ_TIMER1_CAPT  == irq) { timsk(timsk() & ~TICIE1); return; } 
        if (IRQ_TIMER1_COMPA == irq) { timsk(timsk() & ~OCIE1A); return; }  
        if (IRQ_TIMER1_COMPB == irq) { timsk(timsk() & ~OCIE1B); return; } 
        if (IRQ_TIMER1_OVF   == irq) { timsk(timsk() & ~TOIE1); return; } 
	if (IRQ_TIMER1_COMPC == irq) { etimsk(etimsk() & ~OCIE1C); return; }

        if (IRQ_TIMER0_OVF   == irq) { timsk(timsk() & ~TOIE0); return; } 
        if (IRQ_TIMER0_COMP  == irq) { timsk(timsk() & ~OCIE0); return; } 
	
        if (IRQ_SPI_STC      == irq) { spcr(spcr() & ~SPIE); return; }
	
        if (IRQ_USART0_RXC   == irq) { ucsr0b(ucsr0b() & ~RXCIEn); return; } 
        if (IRQ_USART0_UDRE  == irq) { ucsr0b(ucsr0b() & ~UDRIEn); return; } 
        if (IRQ_USART0_TXC   == irq) { ucsr0b(ucsr0b() & ~TXCIEn); return; } 

        if (IRQ_USART1_RXC   == irq) { ucsr1b(ucsr1b() & ~RXCIEn); return; } 
        if (IRQ_USART1_UDRE  == irq) { ucsr1b(ucsr1b() & ~UDRIEn); return; } 
        if (IRQ_USART1_TXC   == irq) { ucsr1b(ucsr1b() & ~TXCIEn); return; }  
	
        if (IRQ_ADC  	     == irq) { adcsra(adcsra() & ~ADIE); return; }
	
        if (IRQ_ANA_COMP     == irq) { acsr(acsr() & ~ACIE); return; }
	
        if (IRQ_EE_RDY       == irq) { eecr(eecr() & ~EERIE); return; }

        if (IRQ_TIMER3_CAPT  == irq) { etimsk(etimsk() & ~TICIE3); return; } 
        if (IRQ_TIMER3_COMPA == irq) { etimsk(etimsk() & ~OCIE3A); return; }  
        if (IRQ_TIMER3_COMPB == irq) { etimsk(etimsk() & ~OCIE3B); return; } 
        if (IRQ_TIMER3_OVF   == irq) { etimsk(etimsk() & ~TOIE3); return; } 
	if (IRQ_TIMER3_COMPC == irq) { etimsk(etimsk() & ~OCIE3C); return; }
    }
    
private:

    typedef AVR8::Reg8 Reg8;

    static Reg8 eimsk() { return AVR8::in8(EIMSK); }
    static void eimsk(Reg8 value) { AVR8::out8(EIMSK,value); } 
    static Reg8 eifr() { return AVR8::in8(EIFR); }
    static void eifr(Reg8 value) { AVR8::out8(EIFR,value); } 
    static Reg8 timsk() { return AVR8::in8(TIMSK); }
    static void timsk(Reg8 value) { AVR8::out8(TIMSK,value); } 
    static Reg8 tifr() { return AVR8::in8(TIFR); }
    static void tifr(Reg8 value) { AVR8::out8(TIFR,value); } 
    static Reg8 etimsk() { return AVR8::in8(ETIMSK); }
    static void etimsk(Reg8 value) { AVR8::out8(ETIMSK,value); } 
    static Reg8 etifr() { return AVR8::in8(ETIFR); }
    static void etifr(Reg8 value) { AVR8::out8(ETIFR,value); } 
    static Reg8 spcr() { return AVR8::in8(SPCR); }
    static void spcr(Reg8 value) { AVR8::out8(SPCR,value); } 
    static Reg8 spsr() { return AVR8::in8(SPSR); }
    static void spsr(Reg8 value) { AVR8::out8(SPSR,value); } 
    static Reg8 ucsr0a() { return AVR8::in8(UCSR0A); }
    static void ucsr0a(Reg8 value) { AVR8::out8(UCSR0A,value); } 
    static Reg8 ucsr0b() { return AVR8::in8(UCSR0B); }
    static void ucsr0b(Reg8 value) { AVR8::out8(UCSR0B,value); } 
    static Reg8 adcsra() { return AVR8::in8(ADCSRA); }
    static void adcsra(Reg8 value) { AVR8::out8(ADCSRA,value); } 
    static Reg8 eecr() { return AVR8::in8(EECR); }
    static void eecr(Reg8 value) { AVR8::out8(EECR,value); } 
    static Reg8 acsr() { return AVR8::in8(ACSR); }
    static void acsr(Reg8 value) { AVR8::out8(ACSR,value); } 
    static Reg8 ucsr1a() { return AVR8::in8(UCSR1A); }
    static void ucsr1a(Reg8 value) { AVR8::out8(UCSR1A,value); } 
    static Reg8 ucsr1b() { return AVR8::in8(UCSR1B); }
    static void ucsr1b(Reg8 value) { AVR8::out8(UCSR1B,value); } 
        
};

typedef ATMega128_IC IC;

__END_SYS

#endif
