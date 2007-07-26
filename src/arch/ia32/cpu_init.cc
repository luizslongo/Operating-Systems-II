// EPOS-- IA32 CPU Mediator Initialization

#include <cpu.h>
#include <mmu.h>
#include <system.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void IA32::init()
{
    db<Init, IA32>(TRC) << "IA32::init()\n";

    // Initialize the MMU
    if(Traits<IA32_MMU>::enabled)
	IA32_MMU::init();
    else
	db<Init, IA32>(WRN) << "MMU is disabled!\n";
}

__END_SYS
