// EPOS-- ATMega128 Mediator Initialization

#include <machine.h>
#include <system.h>

extern "C" unsigned __bss_end;

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void ATMega128::init()
{
    db<Init, ATMega128>(TRC) << "ATMega128::init()\n";

    System_Info<ATMega128> * si = System::info();
    si->pmm.free_base	= (unsigned)&__bss_end;
    si->pmm.free_top	= Memory_Map<Machine>::TOP;

    si->lmm.app_entry =
	reinterpret_cast<unsigned int>(&__epos_library_app_entry);

    IC::disable();

    // Initialize the hardware
    if(Traits<AVR8_MMU>::enabled)
	AVR8_MMU::init();
    if(Traits<AVR8_TSC>::enabled)
	AVR8_TSC::init();

    if(Traits<ATMega128_NIC>::enabled)
	ATMega128_NIC::init();

    return;

}

__END_SYS

