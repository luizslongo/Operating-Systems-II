// EPOS-- Mica2_IC Declarations

#ifndef __mica2_ic_h
#define __mica2_ic_h

#include <ic.h>
#include "../common/avr_ic.h"

__BEGIN_SYS

class Mica2_IC: public AVR_IC
{
private:
    typedef Traits<Mica2_IC> Traits;
    static const Type_Id TYPE	 = Type<Mica2_IC>::TYPE;
    typedef IO_Map<Machine> IO;

    // Mica2_IC private imports, types and constants
    
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

    Mica2_IC()  {};
    ~Mica2_IC() {};
    
    static void enable(const Mask & mask) {

        if (IRQ0  	 & mask) eimsk(eimsk() | INT0);
        if (IRQ1  	 & mask) eimsk(eimsk() | INT1);
        if (IRQ2   	 & mask) eimsk(eimsk() | INT2);
        if (IRQ3   	 & mask) eimsk(eimsk() | INT3);
        if (IRQ4   	 & mask) eimsk(eimsk() | INT4);
        if (IRQ5   	 & mask) eimsk(eimsk() | INT5);
        if (IRQ6   	 & mask) eimsk(eimsk() | INT6);
        if (IRQ7   	 & mask) eimsk(eimsk() | INT7);
	
        if (TIMER2_COMP  & mask) timsk(timsk() | OCIE2);
        if (TIMER2_OVF   & mask) timsk(timsk() | TOIE2);
	
        if (TIMER1_CAPT  & mask) timsk(timsk() | TICIE1); 
        if (TIMER1_COMPA & mask) timsk(timsk() | OCIE1A);  
        if (TIMER1_COMPB & mask) timsk(timsk() | OCIE1B); 
        if (TIMER1_OVF   & mask) timsk(timsk() | TOIE1); 
	if (TIMER1_COMPC & mask) etimsk(etimsk() | OCIE1C);

        if (TIMER0_OVF 	 & mask) timsk(timsk() | TOIE0); 
        if (TIMER0_COMP  & mask) timsk(timsk() | OCIE0); 
	
        if (SPI_STC 	 & mask) spcr(spcr() | SPIE);
	
        if (USART0_RXC   & mask) ucsr0b(ucsr0b() | RXCIEn); 
        if (USART0_UDRE  & mask) ucsr0b(ucsr0b() | UDRIEn); 
        if (USART0_TXC 	 & mask) ucsr0b(ucsr0b() | TXCIEn); 

        if (USART1_RXC   & mask) ucsr1b(ucsr1b() | RXCIEn); 
        if (USART1_UDRE  & mask) ucsr1b(ucsr1b() | UDRIEn); 
        if (USART1_TXC 	 & mask) ucsr1b(ucsr1b() | TXCIEn);  

        if (ADC  	 & mask) adcsra(adcsra() | ADIE);
	
        if (ANA_COMP   	 & mask) acsr(acsr() | ACIE);
	
        if (EE_RDY   	 & mask) eecr(eecr() | EERIE);

        if (TIMER3_CAPT  & mask) etimsk(etimsk() | TICIE3); 
        if (TIMER3_COMPA & mask) etimsk(etimsk() | OCIE3A);  
        if (TIMER3_COMPB & mask) etimsk(etimsk() | OCIE3B); 
        if (TIMER3_OVF   & mask) etimsk(etimsk() | TOIE3); 
	if (TIMER3_COMPC & mask) etimsk(etimsk() | OCIE3C);
    
    }
    
    static void disable(const Mask & mask) {

        if (IRQ0  	 & mask) eimsk(eimsk() & ~INT0);
        if (IRQ1  	 & mask) eimsk(eimsk() & ~INT1);
        if (IRQ2   	 & mask) eimsk(eimsk() & ~INT2);
        if (IRQ3   	 & mask) eimsk(eimsk() & ~INT3);
        if (IRQ4   	 & mask) eimsk(eimsk() & ~INT4);
        if (IRQ5   	 & mask) eimsk(eimsk() & ~INT5);
        if (IRQ6   	 & mask) eimsk(eimsk() & ~INT6);
        if (IRQ7   	 & mask) eimsk(eimsk() & ~INT7);
	
        if (TIMER2_COMP  & mask) timsk(timsk() & ~OCIE2);
        if (TIMER2_OVF   & mask) timsk(timsk() & ~TOIE2);
	
        if (TIMER1_CAPT  & mask) timsk(timsk() & ~TICIE1); 
        if (TIMER1_COMPA & mask) timsk(timsk() & ~OCIE1A);  
        if (TIMER1_COMPB & mask) timsk(timsk() & ~OCIE1B); 
        if (TIMER1_OVF   & mask) timsk(timsk() & ~TOIE1); 
	if (TIMER1_COMPC & mask) etimsk(etimsk() & ~OCIE1C);

        if (TIMER0_OVF 	 & mask) timsk(timsk() & ~TOIE0); 
        if (TIMER0_COMP  & mask) timsk(timsk() & ~OCIE0); 
	
        if (SPI_STC 	 & mask) spcr(spcr() & ~SPIE);
	
        if (USART0_RXC   & mask) ucsr0b(ucsr0b() & ~RXCIEn); 
        if (USART0_UDRE  & mask) ucsr0b(ucsr0b() & ~UDRIEn); 
        if (USART0_TXC 	 & mask) ucsr0b(ucsr0b() & ~TXCIEn); 

        if (USART1_RXC   & mask) ucsr1b(ucsr1b() & ~RXCIEn); 
        if (USART1_UDRE  & mask) ucsr1b(ucsr1b() & ~UDRIEn); 
        if (USART1_TXC 	 & mask) ucsr1b(ucsr1b() & ~TXCIEn);  
	
        if (ADC  	 & mask) adcsra(adcsra() & ~ADIE);
	
        if (ANA_COMP   	 & mask) acsr(acsr() & ~ACIE);
	
        if (EE_RDY   	 & mask) eecr(eecr() & ~EERIE);

        if (TIMER3_CAPT  & mask) etimsk(etimsk() & ~TICIE3); 
        if (TIMER3_COMPA & mask) etimsk(etimsk() & ~OCIE3A);  
        if (TIMER3_COMPB & mask) etimsk(etimsk() & ~OCIE3B); 
        if (TIMER3_OVF   & mask) etimsk(etimsk() & ~TOIE3); 
	if (TIMER3_COMPC & mask) etimsk(etimsk() & ~OCIE3C);

    }
    
    static Mask pending();
    
    static Mask enabled();
    
    static Mask disabled(){ return ~enabled(); }
    
    static int init(System_Info *si);

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

typedef Mica2_IC IC;

__END_SYS

#endif
