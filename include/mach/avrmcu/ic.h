// EPOS-- AVRMCU_IC Declarations

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.


#ifndef __avrmcu_ic_h
#define __avrmcu_ic_h

#include <ic.h>

__BEGIN_SYS

class AVRMCU_IC: public IC_Common
{
private:
    typedef Traits<AVRMCU_IC> Traits;
    static const Type_Id TYPE = Type<AVRMCU_IC>::TYPE;

    // AVRMCU_IC private imports, types and constants
    
    // Interrupt IO Registers
    enum {	
	GICR  = 0x3b,
	GIFR  = 0x3a,
	TIMSK = 0x39,
	TIFR = 0x38,
	TWCR = 0x36,
	MCUCR = 0x35,
	MCUCSR = 0x34,
	EECR = 0x1c,
	SPSR = 0x0e,
	SPCR = 0x0d,
	UCSRA = 0x0b,
	UCSRB = 0x0a,
	ACSR = 0x08,
	ADCSRA = 0x06
    };    
    
    // Interrupt Register Flags
    enum {
    	// GICR
	INT1 = 0x80,
	INT0 = 0x40,
	INT2 = 0x20,
	IVSEL = 0x02,
	IVCE = 0x01,
	// GIFR
	INTF1 = 0x80,
	INTF0 = 0x40,
	INTF2 = 0x20,
	// TIMSK
	OCIE2 = 0x80,
	TOIE2 = 0x40,
	TICIE1 = 0x20,
	OCIE1A = 0x10,
	OCIE1B = 0x08,
	TOIE1 = 0x04,
	OCIE0 = 0x02,
	TOIE0 = 0x01,
	// TIFR
	OCF2 = 0x80,
	TOV2 = 0x40,
	ICF1 = 0x20,
	OCF1A = 0x10,
	OCF1B = 0x08,
	TOV1 = 0x04,
	OCF0 = 0x02,
	TOV0 = 0x01,
	// TWCR
	TWINT = 0x80,
	TWIE = 0x01,
	// MCUCR
	ISC11 = 0x08,
	ISC10 = 0x04,
	ISC01 = 0x02,
	ICS00 = 0x01,
	// MCUCSR
	ISC2 = 0x40,
	// EECR
	EERIE = 0x80,
	// SPSR
	SPIF = 0x80,
	// SPCR
	SPIE = 0x80,
	// UCSRA
	RXC = 0x80,
	TXC = 0x40,
	UDRE = 0x20,
	// UCSRB
	RXCIE = 0x80,
	TXCIE = 0x40,
	UDRIE = 0x20,
	// ACSR
	ACI = 0x10,
	ACIE = 0x80,
	//ADCSRA
	ADIF = 0x10,
	ADIE = 0x80
    };

public:

    // IRQs
    enum {
    	RESET 		= 0x000001,
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
    
    void enable(const Mask & m);
    
    void disable(const Mask & m);
    
    Mask pending();
    
    Mask servicing();
    
    Mask enabled() ;
    
    Mask disabled(){ return ~enabled(); };
    
    void eoi(){};


    static int init(System_Info *si);

private:
    // AVRMCU_IC implementation methods

private:
    // AVRMCU_IC attributes
};

__END_SYS

#endif
