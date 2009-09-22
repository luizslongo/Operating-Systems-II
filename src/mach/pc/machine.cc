// EPOS-- PC Mediator Implementation

#include <mach/pc/machine.h>

__BEGIN_SYS

// Class attributes
volatile unsigned int PC::_n_cpus;
volatile bool PC::_bp_finished;
volatile unsigned int PC::_ap_finished;

// Class methods
void PC::panic()
{
    CPU::int_disable(); 
    Display::position(24, 73);
    Display::puts("PANIC!");
    CPU::halt();
}

__END_SYS
