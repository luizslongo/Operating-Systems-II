// EPOS-- AVRMCU_IC Declarations

#ifndef __avrmcu_ic_h
#define __avrmcu_ic_h

#include <ic.h>
//#include <machine.h>

__BEGIN_SYS

class AVRMCU_IC: public IC_Common
{
private:
    typedef Traits<AVRMCU_IC> Traits;
    static const Type_Id TYPE	 = Type<AVRMCU_IC>::TYPE;

    // AVRMCU_IC private imports, types and constants
    
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
	EERIE	 = 0x80,
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

    AVRMCU_IC()  {};
    ~AVRMCU_IC() {};
    
    void map(unsigned char b);
    
    void enable(const Mask & mask) {
        
        if (IRQ0  	 & mask) gicr(gicr() | INT0);
        if (IRQ1  	 & mask) gicr(gicr() | INT1);
        if (IRQ2   	 & mask) gicr(gicr() | INT2);
	
        if (TIMER2_COMP  & mask) timsk(timsk() | OCIE2);
        if (TIMER2_OVF   & mask) timsk(timsk() | TOIE2);
	
        if (TIMER1_CAPT  & mask) timsk(timsk() | TICIE1); 
        if (TIMER1_COMPA & mask) timsk(timsk() | OCIE1A);  
        if (TIMER1_COMPB & mask) timsk(timsk() | OCIE1B); 
        if (TIMER1_OVF   & mask) timsk(timsk() | TOIE1); 

        if (TIMER0_OVF 	 & mask) timsk(timsk() | TOIE0); 
        if (TIMER0_COMP  & mask) timsk(timsk() | OCIE0); 
	
        if (SPI_STC 	 & mask) spcr(spcr() | SPIE);
	
        if (USART_RXC  	 & mask) ucsrb(ucsrb() | RXCIE); 
        if (USART_UDRE 	 & mask) ucsrb(ucsrb() | UDRIE); 
        if (USART_TXC 	 & mask) ucsrb(ucsrb() | TXCIE); 
	
        if (ADC  	 & mask) adcsra(adcsra() | ADIE);
	
        if (ANA_COMP   	 & mask) acsr(acsr() | ACIE);
	
        if (EE_RDY   	 & mask) eecr(eecr() | EERIE);
	
        if (TWI   	 & mask) twcr(twcr() | TWIE);
	
	// if (RESET  & mask);   // Always enabled
        // if (SPM_RDY  & mask); // Unused  
	
	return;      
    
    }
    
    void disable(const Mask & mask) {

        if (IRQ0  	 & mask) gicr(gicr() & ~INT0);
        if (IRQ1  	 & mask) gicr(gicr() & ~INT1);
        if (IRQ2   	 & mask) gicr(gicr() & ~INT2);
	
        if (TIMER2_COMP  & mask) timsk(timsk() & ~OCIE2);
        if (TIMER2_OVF   & mask) timsk(timsk() & ~TOIE2);
	
        if (TIMER1_CAPT  & mask) timsk(timsk() & ~TICIE1); 
        if (TIMER1_COMPA & mask) timsk(timsk() & ~OCIE1A);  
        if (TIMER1_COMPB & mask) timsk(timsk() & ~OCIE1B); 
        if (TIMER1_OVF   & mask) timsk(timsk() & ~TOIE1); 

        if (TIMER0_OVF 	 & mask) timsk(timsk() & ~TOIE0); 
        if (TIMER0_COMP  & mask) timsk(timsk() & ~OCIE0); 
	
        if (SPI_STC 	 & mask) spcr(spcr() & ~SPIE);
	
        if (USART_RXC  	 & mask) ucsrb(ucsrb() & ~RXCIE); 
        if (USART_UDRE 	 & mask) ucsrb(ucsrb() & ~UDRIE); 
        if (USART_TXC 	 & mask) ucsrb(ucsrb() & ~TXCIE); 
	
        if (ADC  	 & mask) adcsra(adcsra() & ~ADIE);
	
        if (ANA_COMP   	 & mask) acsr(acsr() & ~ACIE);
	
        if (EE_RDY   	 & mask) eecr(eecr() & ~EERIE);
	
        if (TWI   	 & mask) twcr(twcr() & ~TWIE);
	
	// if (RESET  & mask);   // Always enabled
        // if (SPM_RDY  & mask); // Unused        
    
	return;
    
    }
    
    Mask pending();
    
    Mask servicing() {return 0;}
    
    Mask enabled();
    
    Mask disabled(){ return ~enabled(); };
    
    void eoi(){};

    static int init(System_Info *si);

private:

    typedef AVR8::Reg8 Reg8;

    static Reg8 gicr(){
	return AVR8::in8(GICR);
    }
    
    static void gicr(Reg8 value){
	AVR8::out8(GICR,value);
    }    
    
    static Reg8 gifr(){
	return AVR8::in8(GIFR);
    }
    
    static void gifr(Reg8 value){
	AVR8::out8(GIFR,value);
    } 
    
    static Reg8 timsk(){
	return AVR8::in8(TIMSK);
    }
    
    static void timsk(Reg8 value){
	AVR8::out8(TIMSK,value);
    } 
    
    static Reg8 tifr(){
	return AVR8::in8(TIFR);
    }
    
    static void tifr(Reg8 value){
	AVR8::out8(TIFR,value);
    } 
    
    static Reg8 twcr(){
	return AVR8::in8(TWCR);
    }
    
    static void twcr(Reg8 value){
	AVR8::out8(TWCR,value);
    } 
    
    static Reg8 mcucr(){
	return AVR8::in8(MCUCR);
    }
    
    static void mcucr(Reg8 value){
	AVR8::out8(MCUCR,value);
    } 
    
    static Reg8 mcucsr(){
	return AVR8::in8(MCUCSR);
    }
    
    static void mcucsr(Reg8 value){
	AVR8::out8(MCUCSR,value);
    } 
    
    static Reg8 eecr(){
	return AVR8::in8(EECR);
    }
    
    static void eecr(Reg8 value){
	AVR8::out8(EECR,value);
    } 
    
    static Reg8 spsr(){
	return AVR8::in8(SPSR);
    }
    
    static void spsr(Reg8 value){
	AVR8::out8(SPSR,value);
    } 
    
    static Reg8 spcr(){
	return AVR8::in8(SPCR);
    }
    
    static void spcr(Reg8 value){
	AVR8::out8(SPCR,value);
    }                                     
    
    static Reg8 ucsra(){
	return AVR8::in8(UCSRA);
    }
    
    static void ucsra(Reg8 value){
	AVR8::out8(UCSRA,value);
    }       
    
    static Reg8 ucsrb(){
	return AVR8::in8(UCSRB);
    }
    
    static void ucsrb(Reg8 value){
	AVR8::out8(UCSRB,value);
    }       
    
    static Reg8 acsr(){
	return AVR8::in8(ACSR);
    }
    
    static void acsr(Reg8 value){
	AVR8::out8(ACSR,value);
    }          
    
    static Reg8 adcsra(){
	return AVR8::in8(ADCSRA);
    }
    
    static void adcsra(Reg8 value){
	AVR8::out8(ADCSRA,value);
    }          
        
};

__END_SYS

#endif
