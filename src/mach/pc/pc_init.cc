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

    // Set all ISRs to int_not()
    for(int i = 0; i <= IA32::EXC_LAST; i++)
 	int_handler(i, PC::isr_wrapper<int_not>);

    // Set all FSRs to panic()
    for(int i = IA32::EXC_LAST + 1; i < IA32::IDT_ENTRIES; i++)
 	int_handler(i, PC::fsr_wrapper<exc_not>);
    
    // Reset some important FSRs
    int_handler(IA32::EXC_PF,    PC::fsr_wrapper<exc_pf>);
    int_handler(IA32::EXC_GPF,   PC::fsr_wrapper<exc_gpf>);
    int_handler(IA32::EXC_NODEV, PC::fsr_wrapper<exc_fpu>);

    return 0;
}

__END_SYS
