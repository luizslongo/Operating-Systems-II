// EPOS-- ATMega128_IC Implementation

#include <mach/atmega128/ic.h>

__BEGIN_SYS

ATMega128_IC::Mask ATMega128_IC::enabled() {

    Mask mask = 0; 

    if (eimsk() & INT0)    mask |= IRQ0;
    if (eimsk() & INT1)    mask |= IRQ1;
    if (eimsk() & INT2)    mask |= IRQ2;
    if (eimsk() & INT3)    mask |= IRQ3;
    if (eimsk() & INT4)    mask |= IRQ4;
    if (eimsk() & INT5)    mask |= IRQ5;
    if (eimsk() & INT6)    mask |= IRQ6;
    if (eimsk() & INT7)    mask |= IRQ7;

    if (timsk() & OCIE2)   mask |= TIMER2_COMP;
    if (timsk() & TOIE2)   mask |= TIMER2_OVF;

    if (timsk() & TICIE1)  mask |= TIMER1_CAPT;
    if (timsk() & OCIE1A)  mask |= TIMER1_COMPA;
    if (timsk() & OCIE1B)  mask |= TIMER1_COMPB;
    if (timsk() & TOIE1)   mask |= TIMER1_OVF;
    if (etimsk() & OCIE1C) mask |= TIMER1_COMPC;

    if (timsk() & TOIE0)   mask |= TIMER0_OVF;
    if (timsk() & OCIE0)   mask |= TIMER0_COMP;

    if (spcr() & SPIE)     mask |= SPI_STC;

    if (ucsr0b() & RXCIEn) mask |= USART0_RXC;
    if (ucsr0b() & UDRIEn) mask |= USART0_UDRE;
    if (ucsr0b() & TXCIEn) mask |= USART0_TXC;

    if (ucsr1b() & RXCIEn) mask |= USART1_RXC;
    if (ucsr1b() & UDRIEn) mask |= USART1_UDRE;
    if (ucsr1b() & TXCIEn) mask |= USART1_TXC;

    if (adcsra() & ADIE)   mask |= ADC;

    if (acsr() & ACIE)     mask |= ANA_COMP;

    if (eecr() & EERIE)    mask |= EE_RDY;

    if (etimsk() & TICIE3) mask |= TIMER3_CAPT;
    if (etimsk() & OCIE3A) mask |= TIMER3_COMPA;
    if (etimsk() & OCIE3B) mask |= TIMER3_COMPB;
    if (etimsk() & TOIE3)  mask |= TIMER3_OVF;
    if (etimsk() & OCIE3C) mask |= TIMER3_COMPC;
	
    return mask;
};

ATMega128_IC::Mask ATMega128_IC::pending() {

    Mask mask = 0; 

    if (eifr() & INTF0)    mask |= IRQ0;
    if (eifr() & INTF1)    mask |= IRQ1;
    if (eifr() & INTF2)    mask |= IRQ2;
    if (eifr() & INTF3)    mask |= IRQ3;
    if (eifr() & INTF4)    mask |= IRQ4;
    if (eifr() & INTF5)    mask |= IRQ5;
    if (eifr() & INTF6)    mask |= IRQ6;
    if (eifr() & INTF7)    mask |= IRQ7;

    if (tifr() & OCF2)     mask |= TIMER2_COMP;
    if (tifr() & TOV2)     mask |= TIMER2_OVF;

    if (tifr() & ICF1)     mask |= TIMER1_CAPT;
    if (tifr() & OCF1A)    mask |= TIMER1_COMPA;
    if (tifr() & OCF1B)    mask |= TIMER1_COMPB;
    if (tifr() & TOV1)     mask |= TIMER1_OVF;
    if (etifr() & OCF1C)   mask |= TIMER1_COMPC;

    if (tifr() & TOV0)     mask |= TIMER0_OVF;
    if (tifr() & OCF0)     mask |= TIMER0_COMP;

    if (spsr() & SPIF)     mask |= SPI_STC;

    if (ucsr0a() & RXCn)   mask |= USART0_RXC;
    if (ucsr0a() & UDREn)  mask |= USART0_UDRE;
    if (ucsr0a() & TXCn)   mask |= USART0_TXC;

    if (ucsr1a() & RXCn)   mask |= USART1_RXC;
    if (ucsr1a() & UDREn)  mask |= USART1_UDRE;
    if (ucsr1a() & TXCn)   mask |= USART1_TXC;

    if (adcsra() & ADIF)   mask |= ADC;

    if (acsr() & ACI)      mask |= ANA_COMP;

    if (etifr() & ICF3)    mask |= TIMER3_CAPT;
    if (etifr() & OCF3A)   mask |= TIMER3_COMPA;
    if (etifr() & OCF3B)   mask |= TIMER3_COMPB;
    if (etifr() & TOV3)    mask |= TIMER3_OVF;
    if (etifr() & OCF3C)   mask |= TIMER3_COMPC;

    return mask;
    
}





__END_SYS
