// EPOS-- AVRMCU_IC Implementation

#include <mach/avrmcu/ic.h>

__BEGIN_SYS

void AVRMCU_IC::enable(const Mask & mask) {

        
        if (IRQ0  	 & mask) AVR8::out8(GICR,(INT0 | AVR8::in8(GICR)));
        if (IRQ1  	 & mask) AVR8::out8(GICR,(INT1 | AVR8::in8(GICR)));
        if (IRQ2   	 & mask) AVR8::out8(GICR,(INT2 | AVR8::in8(GICR)));
	
        if (TIMER2_COMP  & mask) AVR8::out8(TIMSK,(OCIE2 | AVR8::in8(TIMSK)));
        if (TIMER2_OVF   & mask) AVR8::out8(TIMSK,(TOIE2 | AVR8::in8(TIMSK)));
	
        if (TIMER1_CAPT  & mask) AVR8::out8(TIMSK,(TICIE1 | AVR8::in8(TIMSK)));
        if (TIMER1_COMPA & mask) AVR8::out8(TIMSK,(OCIE1A | AVR8::in8(TIMSK)));
        if (TIMER1_COMPB & mask) AVR8::out8(TIMSK,(OCIE1B | AVR8::in8(TIMSK)));
        if (TIMER1_OVF   & mask) AVR8::out8(TIMSK,(TOIE1 | AVR8::in8(TIMSK)));

        if (TIMER0_OVF 	 & mask) AVR8::out8(TIMSK,(TOIE0 | AVR8::in8(TIMSK)));
        if (TIMER0_COMP  & mask) AVR8::out8(TIMSK,(OCIE0 | AVR8::in8(TIMSK)));
	
        if (SPI_STC 	 & mask) AVR8::out8(SPCR,(SPIE | AVR8::in8(SPCR)));
	
        if (USART_RXC  	 & mask) AVR8::out8(UCSRB,(RXCIE | AVR8::in8(UCSRB)));
        if (USART_UDRE 	 & mask) AVR8::out8(UCSRB,(UDRIE | AVR8::in8(UCSRB)));
        if (USART_TXC 	 & mask) AVR8::out8(UCSRB,(TXCIE | AVR8::in8(UCSRB)));
	
        if (ADC  	 & mask) AVR8::out8(ADCSRA,(ADIE | AVR8::in8(ADCSRA)));
	
        if (ANA_COMP   	 & mask) AVR8::out8(ACSR,(ACIE | AVR8::in8(ACSR)));
	
        if (EE_RDY   	 & mask) AVR8::out8(EECR,(EERIE | AVR8::in8(EECR)));
	
        if (TWI   	 & mask) AVR8::out8(TWCR,(TWIE | AVR8::in8(TWCR)));
	
	AVR8::int_enable();
	

	// if (RESET  & mask);   // Always enabled
        // if (SPM_RDY  & mask); // Unused         
};


void AVRMCU_IC::disable(const Mask & mask) {

        
        if (IRQ0  	 & mask) AVR8::out8(GICR,(~INT0 & AVR8::in8(GICR)));
        if (IRQ1  	 & mask) AVR8::out8(GICR,(~INT1 & AVR8::in8(GICR)));
	if (IRQ2   	 & mask) AVR8::out8(GICR,(~INT2 & AVR8::in8(GICR)));
	
        if (TIMER2_COMP  & mask) AVR8::out8(TIMSK,(~OCIE2 & AVR8::in8(TIMSK)));
        if (TIMER2_OVF   & mask) AVR8::out8(TIMSK,(~TOIE2 & AVR8::in8(TIMSK)));
	
        if (TIMER1_CAPT  & mask) AVR8::out8(TIMSK,(~TICIE1 & AVR8::in8(TIMSK)));
        if (TIMER1_COMPA & mask) AVR8::out8(TIMSK,(~OCIE1A & AVR8::in8(TIMSK)));
        if (TIMER1_COMPB & mask) AVR8::out8(TIMSK,(~OCIE1B & AVR8::in8(TIMSK)));
        if (TIMER1_OVF   & mask) AVR8::out8(TIMSK,(~TOIE1 & AVR8::in8(TIMSK)));

        if (TIMER0_OVF 	 & mask) AVR8::out8(TIMSK,(~TOIE0 & AVR8::in8(TIMSK)));
	if (TIMER0_COMP  & mask) AVR8::out8(TIMSK,(~OCIE0 & AVR8::in8(TIMSK)));
	
        if (SPI_STC 	 & mask) AVR8::out8(SPCR,(~SPIE & AVR8::in8(SPCR)));
	
        if (USART_RXC  	 & mask) AVR8::out8(UCSRB,(~RXCIE & AVR8::in8(UCSRB)));
        if (USART_UDRE 	 & mask) AVR8::out8(UCSRB,(~UDRIE & AVR8::in8(UCSRB)));
        if (USART_TXC 	 & mask) AVR8::out8(UCSRB,(~TXCIE & AVR8::in8(UCSRB)));
	
        if (ADC  	 & mask) AVR8::out8(ADCSRA,(~ADIE & AVR8::in8(ADCSRA)));

        if (ANA_COMP   	 & mask) AVR8::out8(ACSR,(~ACIE & AVR8::in8(ACSR)));

        if (EE_RDY   	 & mask) AVR8::out8(EECR,(~EERIE & AVR8::in8(EECR)));
	
        if (TWI   	 & mask) AVR8::out8(TWCR,(~TWIE & AVR8::in8(TWCR)));
	

	// if (RESET  & mask);   // Always enabled
        // if (SPM_RDY  & mask); // Unused     
	
	AVR8::int_disable();    
};


IC_Common::Mask AVRMCU_IC::enabled() {

	Mask mask = RESET; // Reset always enabled

        if (INT0 & AVR8::in8(GICR))	mask |= INT0;
        if (INT1 & AVR8::in8(GICR))	mask |= INT1;
	if (INT2 & AVR8::in8(GICR))	mask |= INT2;
	
        if (OCIE2 & AVR8::in8(TIMSK)) 	mask |= OCIE2;
        if (TOIE2 & AVR8::in8(TIMSK))	mask |= TOIE2;
	
        if (TICIE1 & AVR8::in8(TIMSK))	mask |= TICIE1;
        if (OCIE1A & AVR8::in8(TIMSK))	mask |= OCIE1A;
        if (OCIE1B & AVR8::in8(TIMSK))	mask |= OCIE1B;
        if (TOIE1 & AVR8::in8(TIMSK))	mask |= TOIE1;

        if (TOIE0 & AVR8::in8(TIMSK))	mask |= TOIE0;
	if (OCIE0 & AVR8::in8(TIMSK))	mask |= OCIE0;
	
        if (SPIE & AVR8::in8(SPCR))	mask |= SPIE;
	
        if (RXCIE & AVR8::in8(UCSRB))	mask |= RXCIE;
        if (UDRIE & AVR8::in8(UCSRB))	mask |= UDRIE;
        if (TXCIE & AVR8::in8(UCSRB))	mask |= TXCIE;
	
        if (ADIE & AVR8::in8(ADCSRA))	mask |= ADIE;

        if (ACIE & AVR8::in8(ACSR))	mask |= ACIE;

        if (EERIE & AVR8::in8(EECR))	mask |= EERIE;
	
        if (TWIE & AVR8::in8(TWCR))	mask |= TWIE;

        return(mask);
};

IC_Common::Mask AVRMCU_IC::pending() {
    return 0;
}





__END_SYS
