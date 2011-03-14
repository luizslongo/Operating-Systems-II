// EPOS-- ARM7 CPU Mediator Initialization

#include <cpu.h>
#include <mmu.h>
#include <tsc.h>

__BEGIN_SYS

void ARM7::init()
{
	MMU::init();
	return;
}

__END_SYS
