// EPOS-- ML310 Timer Mediator

#include <mach/ml310/timer.h>

__BEGIN_SYS

int ML310_Timer::init(System_Info * si)
{
    //db<ML310_Timer>(TRC)
    kout << "ML310_Timer::init()\n";

    CPU::_mtspr(SPR_PIT, 0); //PIT = 0
    CPU::_mtspr(SPR_TSR, 0xFFFFFFFF); //Disable all status (TSR)
    CPU::_mtspr(SPR_TCR, 0x04400000); //Enable Timer with PIT AUTO RELOAD ENABLED. (TCR)

    return 0;
}

__END_SYS
