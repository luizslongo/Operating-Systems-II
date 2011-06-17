// EPOS AIX4LITE Mediator Initialization

#include <machine.h>
#include <system.h>
#include <uart.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void AIX4LITE::init()
{
    db<AIX4LITE>(TRC) << "AIX4LITE::init()\n";

    if(Traits<AIX4LITE_IC>::enabled)
	AIX4LITE_IC::init();
    if(Traits<AIX4LITE_Timer>::enabled)
	AIX4LITE_Timer::init();
    if(Traits<AIX4LITE_UART>::enabled)
	AIX4LITE_UART::init();
    if(Traits<AIX4LITE_NIC>::enabled)
	AIX4LITE_NIC::init();
}

__END_SYS
