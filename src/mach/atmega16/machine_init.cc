// EPOS-- ATMega16 Mediator Initialization

#include <machine.h>
#include <system.h>

extern "C" unsigned __bss_end;

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void ATMega16::init()
{
    db<Init, ATMega16>(TRC) << "ATMega16::init()\n";

    System_Info<ATMega16> * si = System::info();
    si->pmm.free_base	= (unsigned)&__bss_end;
    si->pmm.free_top	= Memory_Map<Machine>::TOP;

    si->lmm.app_entry =
	reinterpret_cast<unsigned int>(&__epos_library_app_entry);

    // Initialize the hardware
    if(Traits<AVR8>::enabled)
	AVR8::init();
    if(Traits<AVR8_MMU>::enabled)
	AVR8_MMU::init();
    if(Traits<AVR8_TSC>::enabled)
	AVR8_TSC::init();

    if(Traits<ATMega16_NIC>::enabled)
	ATMega16_NIC::init();

    return;

}

__END_SYS

