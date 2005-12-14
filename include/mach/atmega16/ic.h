// EPOS-- ATMega16 Interrupt Controller Mediator Declarations

#ifndef __atmega16_ic_h
#define __atmega16_ic_h

#include "../avr_common/ic.h"

__BEGIN_SYS

class ATMega16_IC: public IC_Common, private AVR_IC
{
private:
    typedef AVR8::Reg8 Reg8;

    // Interrupt IO Registers
    enum {	
	GICR  	= 0x3b,
	GIFR  	= 0x3a,
	TIMSK 	= 0x39,
	TIFR 	= 0x38,
	TWCR 	= 0x36,
	MCUCR 	= 0x35,
	MCUCSR 	= 0x34,
	EECR 	= 0x1c,
	SPSR 	= 0x0e,
	SPCR 	= 0x0d,
	UCSRA 	= 0x0b,
	UCSRB 	= 0x0a,
	ACSR 	= 0x08,
	ADCSRA 	= 0x06
    };    
    
    // Interrupt Register Flags
    enum {
    	// GICR
	INT1	 = 0x80,
	INT0	 = 0x40,
	INT2	 = 0x20,
	IVSEL	 = 0x02,
	IVCE	 = 0x01,
	// GIFR
	INTF1	 = 0x80,
	INTF0	 = 0x40,
	INTF2	 = 0x20,
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
	// TWCR
	TWINT	 = 0x80,
	TWIE	 = 0x01,
	// MCUCR
	ISC11	 = 0x08,
	ISC10	 = 0x04,
	ISC01	 = 0x02,
	ICS00	 = 0x01,
	// MCUCSR
	ISC2	 = 0x40,
	// EECR
	EERIE	 = 0x40,
	// SPSR
	SPIF	 = 0x80,
	// SPCR
	SPIE	 = 0x80,
	// UCSRA
	RXC	 = 0x80,
	TXC	 = 0x40,
	UDRE	 = 0x20,
	// UCSRB
	RXCIE	 = 0x80,
	TXCIE	 = 0x40,
	UDRIE	 = 0x20,
	// ACSR
	ACI	 = 0x10,
	ACIE	 = 0x80,
	//ADCSRA
	ADIF	 = 0x10,
	ADIE	 = 0x80
    };

public:
    // IRQs
    enum {
    	RESET		= 0x000001,
	IRQ0 		= 0x000002,
	IRQ1		= 0x000004,
	TIMER2_COMP	= 0x000008,
	TIMER2_OVF	= 0x000010,
	TIMER1_CAPT	= 0x000020,
	TIMER1_COMPA	= 0x000040,
	TIMER1_COMPB	= 0x000080,
	TIMER1_OVF	= 0x000100,
	TIMER0_OVF	= 0x000200,
	SPI_STC		= 0x000400,
	USART_RXC	= 0x000800,
	USART_UDRE	= 0x001000,
	USART_TXC	= 0x002000,
	ADC		= 0x004000,
	EE_RDY		= 0x008000,
	ANA_COMP	= 0x010000,
	TWI		= 0x020000,
	IRQ2		= 0x040000,
	TIMER0_COMP	= 0x080000,
	SPM_RDY		= 0x100000   
    };

    enum {
	IRQ_RESET        = 0,
	IRQ_IRQ0         = 1,
	IRQ_IRQ1         = 2,
	IRQ_TIMER2_COMP  = 3,
	IRQ_TIMER2_OVF   = 4,
	IRQ_TIMER1_CAPT  = 5,
	IRQ_TIMER1_COMPA = 6,
	IRQ_TIMER1_COMPB = 7,
	IRQ_TIMER1_OVF   = 8,
	IRQ_TIMER0_OVF   = 9,
	IRQ_SPI_STC      = 10,
	IRQ_USART0_RXC   = 11,
	IRQ_USART0_UDRE  = 12,
	IRQ_USART0_TXC   = 13,
	IRQ_ADC          = 14,
	IRQ_EE_RDY       = 15,
	IRQ_ANA_COMP     = 16,
	IRQ_TWI          = 17,
	IRQ_IRQ2         = 18,
	IRQ_TIMER0_COMP  = 19,
	IRQ_SPM_RDY      = 20,

    	IRQ_TSC 	 = 100, //not implemented
	IRQ_TIMER 	 = IRQ_TIMER0_COMP
    };

public:
    ATMega16_IC() {};

    static void enable(IRQ irq) {
        
        if (IRQ_IRQ0  	 == irq) { gicr(gicr() | INT0); return; }
        if (IRQ_IRQ1  	 == irq) { gicr(gicr() | INT1); return; }
        if (IRQ_IRQ2   	 == irq) { gicr(gicr() | INT2); return; }
	
        if (IRQ_TIMER2_COMP  == irq) { timsk(timsk() | OCIE2); return; }
        if (IRQ_TIMER2_OVF   == irq) { timsk(timsk() | TOIE2); return; }
	
        if (IRQ_TIMER1_CAPT  == irq) { timsk(timsk() | TICIE1); return; } 
        if (IRQ_TIMER1_COMPA == irq) { timsk(timsk() | OCIE1A); return; }  
        if (IRQ_TIMER1_COMPB == irq) { timsk(timsk() | OCIE1B); return; } 
        if (IRQ_TIMER1_OVF   == irq) { timsk(timsk() | TOIE1); return; } 

        if (IRQ_TIMER0_OVF 	 == irq) { timsk(timsk() | TOIE0); return; } 
        if (IRQ_TIMER0_COMP  == irq) { timsk(timsk() | OCIE0); return; } 
	
        if (IRQ_SPI_STC 	 == irq) { spcr(spcr() | SPIE); return; }
	
        if (IRQ_USART0_RXC  	 == irq) { ucsrb(ucsrb() | RXCIE); return; } 
        if (IRQ_USART0_UDRE 	 == irq) { ucsrb(ucsrb() | UDRIE); return; } 
        if (IRQ_USART0_TXC 	 == irq) { ucsrb(ucsrb() | TXCIE); return; } 
	
        if (IRQ_ADC  	 == irq) { adcsra(adcsra() | ADIE); return; }
	
        if (IRQ_ANA_COMP   	 == irq) { acsr(acsr() | ACIE); return; }
	
        if (IRQ_EE_RDY   	 == irq) { eecr(eecr() | EERIE); return; }
	
        if (IRQ_TWI   	 == irq) { twcr(twcr() | TWIE); return; }
	
	// if (IRQ_RESET  == irq) {return;}   // Always enabled
        // if (IRQ_SPM_RDY  == irq) {return;} // Unused  
    }
    
    static void disable() {
        gicr(gicr() & ~(INT0 | INT1 | INT2));
	
        timsk(timsk() & ~(OCIE2 | TOIE2));
	
        timsk(timsk() & ~(TICIE1 | OCIE1A | OCIE1B | TOIE1)); 

        timsk(timsk() & ~(TOIE0 | OCIE0)); 
	
        spcr(spcr() & ~SPIE);
	
        ucsrb(ucsrb() & ~(RXCIE | UDRIE | TXCIE)); 
	
        adcsra(adcsra() & ~ADIE);
	
        acsr(acsr() & ~ACIE);
	
        eecr(eecr() & ~EERIE);
	
        twcr(twcr() & ~TWIE);
    }
    static void disable(IRQ irq) {

        if (IRQ_IRQ0  	 == irq) { gicr(gicr() & ~INT0); return; }
        if (IRQ_IRQ1  	 == irq) { gicr(gicr() & ~INT1); return; }
        if (IRQ_IRQ2   	 == irq) { gicr(gicr() & ~INT2); return; }
	
        if (IRQ_TIMER2_COMP  == irq) { timsk(timsk() & ~OCIE2); return; }
        if (IRQ_TIMER2_OVF   == irq) { timsk(timsk() & ~TOIE2); return; }
	
        if (IRQ_TIMER1_CAPT  == irq) { timsk(timsk() & ~TICIE1); return; } 
        if (IRQ_TIMER1_COMPA == irq) { timsk(timsk() & ~OCIE1A); return; }  
        if (IRQ_TIMER1_COMPB == irq) { timsk(timsk() & ~OCIE1B); return; } 
        if (IRQ_TIMER1_OVF   == irq) { timsk(timsk() & ~TOIE1); return; } 

        if (IRQ_TIMER0_OVF 	 == irq) { timsk(timsk() & ~TOIE0); return; } 
        if (IRQ_TIMER0_COMP  == irq) { timsk(timsk() & ~OCIE0); return; } 
	
        if (IRQ_SPI_STC 	 == irq) { spcr(spcr() & ~SPIE); return; }
	
        if (IRQ_USART0_RXC  	 == irq) { ucsrb(ucsrb() & ~RXCIE); return; } 
        if (IRQ_USART0_UDRE 	 == irq) { ucsrb(ucsrb() & ~UDRIE); return; } 
        if (IRQ_USART0_TXC 	 == irq) { ucsrb(ucsrb() & ~TXCIE); return; } 
	
        if (IRQ_ADC  	 == irq) { adcsra(adcsra() & ~ADIE); return; }
	
        if (IRQ_ANA_COMP   	 == irq) { acsr(acsr() & ~ACIE); return; }
	
        if (IRQ_EE_RDY   	 == irq) { eecr(eecr() & ~EERIE); return; }
	
        if (IRQ_TWI   	 == irq) { twcr(twcr() & ~TWIE); return; }
	
	// if (IRQ_RESET  == irq) {return;}   // Always enabled
        // if (IRQ_SPM_RDY  == irq) {return;} // Unused        
    }
    
    static int init(System_Info * si) { return 0; }

private:
    static Reg8 gicr() { return AVR8::in8(GICR); }
    static void gicr(Reg8 value) { AVR8::out8(GICR,value); }       
    static Reg8 gifr() { return AVR8::in8(GIFR); }
    static void gifr(Reg8 value) {AVR8::out8(GIFR,value); }     
    static Reg8 timsk() { return AVR8::in8(TIMSK); }
    static void timsk(Reg8 value) {AVR8::out8(TIMSK,value); }    
    static Reg8 tifr() { return AVR8::in8(TIFR); } 
    static void tifr(Reg8 value) {AVR8::out8(TIFR,value); }     
    static Reg8 twcr() { return AVR8::in8(TWCR); }   
    static void twcr(Reg8 value) { AVR8::out8(TWCR,value); }  
    static Reg8 mcucr() { return AVR8::in8(MCUCR); }    
    static void mcucr(Reg8 value) {AVR8::out8(MCUCR,value); }    
    static Reg8 mcucsr() { return AVR8::in8(MCUCSR); }
    static void mcucsr(Reg8 value) {AVR8::out8(MCUCSR,value); }     
    static Reg8 eecr() { return AVR8::in8(EECR); }
    static void eecr(Reg8 value) {AVR8::out8(EECR,value); }  
    static Reg8 spsr() { return AVR8::in8(SPSR); }  
    static void spsr(Reg8 value) {AVR8::out8(SPSR,value); } 
    static Reg8 spcr() { return AVR8::in8(SPCR); }
    static void spcr(Reg8 value) {AVR8::out8(SPCR,value); }                                     
    static Reg8 ucsra() { return AVR8::in8(UCSRA); }
    static void ucsra(Reg8 value) {AVR8::out8(UCSRA,value); }       
    static Reg8 ucsrb() { return AVR8::in8(UCSRB); }    
    static void ucsrb(Reg8 value) {AVR8::out8(UCSRB,value); }       
    static Reg8 acsr() { return AVR8::in8(ACSR); }  
    static void acsr(Reg8 value) {AVR8::out8(ACSR,value); }             
    static Reg8 adcsra() { return AVR8::in8(ADCSRA); }
    static void adcsra(Reg8 value) {AVR8::out8(ADCSRA,value); }          
};

__END_SYS

#endif
