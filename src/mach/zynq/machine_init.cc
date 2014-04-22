#include <mach/zynq/machine.h>
#include <mach/zynq/memory_map.h>
#include <mach/zynq/scu.h>

extern "C" int __epos_library_app_entry();

__BEGIN_SYS

// Since we dont need a setup we will build the system info here
void PandaBoard::init()
{
    db<Init, PandaBoard>(TRC) << "Machine::init()\n";
    
    //kout << "Machine::init()\n";

    System_Info<Zynq> * si;
    
    si = reinterpret_cast<System_Info<Zynq> *>(Memory_Map<Zynq>::SYS_INFO);

    CPU::int_disable();

    // this struct should have meaning info if we change to setup-based init
    si->lm.has_stp = false;
    si->lm.has_ini = false;
    si->lm.has_sys = false;
    si->lm.has_app = true;
    si->lm.has_ext = false;

    si->lm.stp_entry = 0;
    si->lm.stp_code  = 0;
    si->lm.stp_code_size = 0;
    si->lm.stp_data = 0;
    si->lm.stp_data_size = 0;

    si->lm.ini_entry = 0;
    si->lm.ini_code  = 0;
    si->lm.ini_code_size = 0;
    si->lm.ini_data = 0;
    si->lm.ini_data_size = 0;

    si->lm.sys_entry = 0;
    si->lm.sys_code  = 0;
    si->lm.sys_code_size = 0;
    si->lm.sys_data = 0;
    si->lm.sys_data_size = 0;
    si->lm.sys_stack = 0;
    si->lm.sys_stack_size = 0;

    si->lm.app_entry = (unsigned int)&__epos_library_app_entry;
    
	if(Traits<Zynq>::enabled)
		PandaBoard_SCU::init();
    
    if(Traits<PandaBoard_IC>::enabled)
	    PandaBoard_IC::init();
    if(Traits<PandaBoard_Timer>::enabled)
	    PandaBoard_Timer::init();	        
	    
	//kout << "Machine::init() end\n";
}

__END_SYS
