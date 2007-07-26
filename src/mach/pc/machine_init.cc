// EPOS-- PC Mediator Initialization

#include <machine.h>
#include <system.h>

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

    if(Traits<PC_PCI>::enabled)
	PC_PCI::init();
    if(Traits<PC_IC>::enabled)
	PC_IC::init();
    if(Traits<PC_NIC>::enabled)
	PC_NIC::init();
}

__END_SYS
