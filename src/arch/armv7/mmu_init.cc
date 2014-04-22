// EPOS-- ARMV7 MMU Mediator Initialization

#include <mmu.h>
#include <system.h>

extern "C" unsigned __bss_end__;

__BEGIN_SYS

void ARMV7_MMU::init()
{
    db<Init, ARMV7_MMU>(TRC) << "ARMV7_MMU::init()\n";
    
    unsigned int base = (unsigned int)&__bss_end__ + 1;
	if(base % 4 != 0){
		base = base + (4 - base % 4);
	}		

	// let our stack breath!
	const unsigned int limit = 
		Memory_Map<Machine>::TOP - Traits<Machine>::APPLICATION_STACK_SIZE;
		
	//kout << "base = " << base << " limit = " << limit << "\n";
    ARMV7_MMU::free(base, limit - base);
}

__END_SYS

