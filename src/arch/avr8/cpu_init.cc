// EPOS-- AVR8 CPU Mediator Initialization

#include <cpu.h>

__BEGIN_SYS

void AVR8::init()
{
    db<Init, AVR8>(TRC) << "AVR8::init()\n";

    if(_Traits::Power_Management) {
        //Enable sleep modes
        out8(MCUCR, in8(MCUCR) | SE);
	db<AVR8>(INF) << "AVR8:: Power Management Enabled\n";
    }
    else {
        //Disable sleep modes
        out8(MCUCR, in8(MCUCR) & ~SE);
	db<AVR8>(INF) << "AVR8:: Power Management Disabled\n";
    }
}

__END_SYS
