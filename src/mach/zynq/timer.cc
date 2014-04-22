#include <mach/zynq/timer.h>

__BEGIN_SYS

Timer::Handler * PandaBoard_Timer::_handler[3] = { 0, 0, 0};

void PandaBoard_Timer::int_handler(IC::Interrupt_Id id)
{
    //CPU::Reg32 clr = INTERRUPT_CLEAR;
    /*switch (id)
    {
        case IC::TIMERINT0: id = 0; break;
        case IC::TIMERINT1: id = 1; clr += 0x100; break;
        case IC::TIMERINT2: id = 2; clr += 0x200; break;
        default:
            return;
    }*/
    //clear
    switch (id)
    {
        case IC::TIMERINT0: 
            id = 1; //Alarm has ID = 1 
            break;
        //case IC::TIMERINT1: id = 1; clr += 0x100; break;
        //case IC::TIMERINT2: id = 2; clr += 0x200; break;
        default:
            return;
    }        
    
    //call handler
    if (_handler[id]) 
        _handler[id]();
}

__END_SYS



