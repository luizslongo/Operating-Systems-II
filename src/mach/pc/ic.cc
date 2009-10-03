
#include <mach/pc/ic.h>
#include <machine.h>

extern "C" { void _exit(int s); }

__BEGIN_SYS

// Class attributes
APIC::Log_Addr APIC::_base;
PC_IC::Interrupt_Handler PC_IC::_int_vector[PC_IC::INTS];

// Class methods
void PC_IC::int_not(unsigned int i)
{
    db<PC>(WRN) << "\nInt " << i
		<< " occurred, but no handler installed\n";

    Machine::panic();
}

void PC_IC::exc_not(unsigned int i,
		    Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags)
{
    db<PC>(WRN) << "\nAn exception has occurred for which no handler"
		<< " is installed [err=" << (void *)error
		<< ",ctx={cs=" << (void *)cs
		<< ",ip=" << (void *)eip
		<< ",fl=" << (void *)eflags
		<< "}]\n";

    db<PC>(WRN) << "The running thread will now be terminated!\n";
    _exit(-1);
}

void PC_IC::exc_pf(unsigned int i,
		   Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags)
{  
    db<PC>(WRN) << "\nPage fault [address=" << (void *)CPU::cr2()
		<< ",err={";
    if(error & (1 << 0))
	db<PC>(WRN) << "P";
    if(error & (1 << 1))
	db<PC>(WRN) << "W";
    if(error & (1 << 2))
	db<PC>(WRN) << "S";
    if(error & (1 << 3))
	db<PC>(WRN) << "R";
    db<PC>(WRN) << "},ctx={cs=" << (void *)cs
		<< ",ip=" << (void *)eip
		<< ",fl=" << (void *)eflags
		<< "}]\n";

    db<PC>(WRN) << "The running thread will now be terminated!\n";
    _exit(-1);
}

void PC_IC::exc_gpf(unsigned int i,
		    Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags)
{  
    db<PC>(WRN) << "\nGeneral protection fault [err=" << (void *)error
		<< ",ctx={cs=" << (void *)cs
		<< ",ip=" << (void *)eip
		<< ",fl=" << (void *)eflags
		<< "}]\n";

    db<PC>(WRN) << "The running thread will now be terminated!\n";
    _exit(-1);
}

void PC_IC::exc_fpu(unsigned int i,
		    Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags) {  
    db<PC>(WRN) << "\nFPU fault [err=" << (void *)error
		<< ",ctx={cs=" << (void *)cs
		<< ",ip=" << (void *)eip
		<< ",fl=" << (void *)eflags
		<< "}]\n";

    db<PC>(WRN) << "The running thread will now be terminated!\n";
    _exit(-1);
}

__END_SYS
