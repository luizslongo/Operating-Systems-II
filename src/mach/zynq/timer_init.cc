// EPOS PandaBoard Timer Mediator Initialization

#include <timer.h>
//#include <ic.h>

__BEGIN_SYS

void PandaBoard_Timer::init() 
{
    kout << "PandaBoard_Timer::init()\n";
    load(0);
    value(0);        
    control(TIMER_AUTO_RELOAD | TIMER_IT_ENABLE);
    status(INTERRUPT_CLEAR);
    IC::int_vector(IC::TIMERINT0, &PandaBoard_Timer::int_handler);
    //kout << "PandaBoard_Timer::init() calling IC::enable()\n";
    IC::enable(IC::TIMERINT0);
    kout << "PandaBoard_Timer::init() done\n";
}

__END_SYS
