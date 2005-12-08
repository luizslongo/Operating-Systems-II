// EPOS-- ATMega16_IC Implementation

#include <mach/atmega16/ic.h>

__BEGIN_SYS

ATMega16_IC::Mask ATMega16_IC::enabled() {

    Mask mask = RESET; // Reset always enabled

    if (INT0 & gicr())		mask |= INT0;
    if (INT1 & gicr())		mask |= INT1;
    if (INT2 & gicr())		mask |= INT2;
    
    if (OCIE2 & timsk())	mask |= OCIE2;
    if (TOIE2 & timsk())	mask |= TOIE2;
    
    if (TICIE1 & timsk())	mask |= TICIE1;
    if (OCIE1A & timsk())	mask |= OCIE1A;
    if (OCIE1B & timsk())	mask |= OCIE1B;
    if (TOIE1 & timsk())	mask |= TOIE1;

    if (TOIE0 & timsk())	mask |= TOIE0;
    if (OCIE0 & timsk())	mask |= OCIE0;
    
    if (SPIE & spcr())		mask |= SPIE;
    
    if (RXCIE & ucsrb())	mask |= RXCIE;
    if (UDRIE & ucsrb())	mask |= UDRIE;
    if (TXCIE & ucsrb())	mask |= TXCIE;
    
    if (ADIE & adcsra())	mask |= ADIE;

    if (ACIE & acsr())		mask |= ACIE;

    if (EERIE & eecr())		mask |= EERIE;
	
    if (TWIE & twcr())		mask |= TWIE;

    return mask;
};

ATMega16_IC::Mask ATMega16_IC::pending() {

    Mask mask = 0; 

    if (INTF0 & gifr())		mask |= INT0;
    if (INTF1 & gifr())		mask |= INT1;
    if (INTF2 & gifr())		mask |= INT2;
    
    if (OCF2 & tifr())		mask |= OCIE2;
    if (TOV2 & tifr())		mask |= TOIE2;
    
    if (ICF1 & tifr())		mask |= TICIE1;
    if (OCF1A & tifr())		mask |= OCIE1A;
    if (OCF1B & tifr())		mask |= OCIE1B;
    if (TOV1 & tifr())		mask |= TOIE1;

    if (OCF0 & tifr())		mask |= TOIE0;
    if (TOV0 & tifr())		mask |= OCIE0;
    
    if (SPIF & spsr())		mask |= SPIE;
    
    if (RXC & ucsra())		mask |= RXCIE;
    if (UDRE & ucsra())		mask |= UDRIE;
    if (TXC & ucsra())		mask |= TXCIE;
    
    if (ADIF & adcsra())	mask |= ADIE;

    if (ACI & acsr())		mask |= ACIE;

    //if (EERIE & eecr())	mask |= EERIE;
	
    if (TWINT & twcr())		mask |= TWIE;

    return mask;
    
}





__END_SYS
