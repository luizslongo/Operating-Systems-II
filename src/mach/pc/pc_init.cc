// EPOS-- PC Mediator Initialization

#include <mach/pc/pc.h>

__BEGIN_SYS

// Prevent implicit template instantiations
extern template void PC::isr_wrapper<PC::int_not>();
extern template void PC::fsr_wrapper<PC::exc_not>();
extern template void PC::fsr_wrapper<PC::exc_pf>();
extern template void PC::fsr_wrapper<PC::exc_gpf>();
extern template void PC::fsr_wrapper<PC::exc_fpu>();

int PC::init(System_Info * si)
{
    db<PC>(TRC) << "PC::init()\n";

    // Set all IDT entries to panic()
    for(int i = 0; i < IA32::IDT_ENTRIES; i++)
 	int_handler(i, fsr_wrapper<exc_not>);
    
    // Set all ISRs to int_not()
    for(unsigned int i = INT_BASE; i <= INT_BASE + IC::IRQS; i++)
 	int_handler(i, isr_wrapper<int_not>);

    // Reset some important FSRs
    int_handler(IA32::EXC_PF,    fsr_wrapper<exc_pf>);
    int_handler(IA32::EXC_GPF,   fsr_wrapper<exc_gpf>);
    int_handler(IA32::EXC_NODEV, fsr_wrapper<exc_fpu>);

    return 0;
}

__END_SYS
