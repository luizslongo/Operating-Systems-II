// EPOS-- ML310 Mediator Initialization

#include <machine.h>
#include <system.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void ML310::init()
{
    db<ML310>(TRC) << "ML310::init()\n";

    // Set all interrupt handlers to int_not()
    for(unsigned int i = 0; i < INT_VECTOR_SIZE; i++)
       int_vector(i, int_not);

    //Install Processor Exception Handlers
    int_vector(ML310_IC::INT_PROGRAM, exc_program);

    // If EPOS is a library then adjust the application entry point (that
    // was set by SETUP) based on the ELF SYSTEM+APPLICATION image
    System_Info<ML310> * si = System::info();
    if(!si->lm.has_sys)
	si->lmm.app_entry =
	    reinterpret_cast<unsigned int>(&__epos_library_app_entry);

    // Initialize the hardware
    if(Traits<PPC32_MMU>::enabled)
	PPC32_MMU::init();
    else
	db<Init, ML310>(WRN) << "MMU is disabled by traits!\n";

    if(Traits<ML310_PCI>::enabled)
	ML310_PCI::init();
    if(Traits<ML310_IC>::enabled)
	ML310_IC::init();
    if(Traits<ML310_Timer>::enabled)
	ML310_Timer::init();
    if(Traits<ML310_UART>::enabled)
	ML310_UART::init();
    if(Traits<ML310_NIC>::enabled)
	ML310_NIC::init();



}

__END_SYS
