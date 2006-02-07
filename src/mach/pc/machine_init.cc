// EPOS-- PC Mediator Initialization

#include <machine.h>
#include <system.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void PC::init()
{
    db<Init, PC>(TRC) << "PC::init()\n";

    // Set all IDT entries to proper int_dispatch() offsets
    IA32::IDT_Entry * idt =
	reinterpret_cast<IA32::IDT_Entry *>(Memory_Map<PC>::IDT);
    for(unsigned int i = 0; i < IA32::IDT_ENTRIES; i++)
	if(i < INT_VECTOR_SIZE)
	    idt[i] = IA32::IDT_Entry(IA32::GDT_SYS_CODE, 
				     Log_Addr(int_dispatch) + i * 16,
				     IA32::SEG_IDT_ENTRY);
	else
	    idt[i] = IA32::IDT_Entry(IA32::GDT_SYS_CODE, 
				     Log_Addr(int_dispatch) + INT_VECTOR_SIZE,
				     IA32::SEG_IDT_ENTRY);
    
    // Set all interrupt handlers to int_not()
    for(unsigned int i = 0; i < INT_VECTOR_SIZE; i++)
 	_int_vector[i] = int_not;

    // Reset some important exception handlers
    _int_vector[IA32::EXC_PF] = reinterpret_cast<int_handler *>(exc_pf);
    _int_vector[IA32::EXC_DOUBLE] = reinterpret_cast<int_handler *>(exc_pf);
    _int_vector[IA32::EXC_GPF] = reinterpret_cast<int_handler *>(exc_gpf);
    _int_vector[IA32::EXC_NODEV] = reinterpret_cast<int_handler *>(exc_fpu);

    // If EPOS is a library then adjust the application entry point (that
    // was set by SETUP) based on the ELF SYSTEM+APPLICATION image
    System_Info<PC> * si = System::info();
    if(!si->lm.has_sys)
	si->lmm.app_entry =
	    reinterpret_cast<unsigned int>(&__epos_library_app_entry);

    // Initialize the hardware
    if(Traits<IA32_MMU>::enabled)
	IA32_MMU::init();
    else
	db<Init, PC>(WRN) << "MMU is disabled by traits!\n";
	

    if(Traits<PC_PCI>::enabled)
	PC_PCI::init();
    if(Traits<PC_IC>::enabled)
	PC_IC::init();
    if(Traits<PC_NIC>::enabled)
	PC_NIC::init();
}

__END_SYS
