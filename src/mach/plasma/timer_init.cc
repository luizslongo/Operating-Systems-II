// EPOS-- PLASMA Timer Mediator

#include <timer.h>

__BEGIN_SYS

void PLASMA_Timer::init() {
    db<PLASMA_Timer>(TRC) << "PLASMA_Timer::init()\n";
    //PLASMA_IC::enable(PLASMA_IC::IRQ_TIMER);
}

__END_SYS

