// EPOS-- ARM7 CPU Mediator Initialization

#include <cpu.h>
#include <mmu.h>
#include <tsc.h>

__BEGIN_SYS

void ARM7::init()
{
    db<Init, ARM7>(TRC) << "ARM7::init()\n";

	MMU::init();

	return;
}

__END_SYS
