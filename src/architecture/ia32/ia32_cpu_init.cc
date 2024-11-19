// EPOS IA32 CPU Mediator Initialization

#include <architecture.h>
#include <machine/machine.h>
#include <system.h>

__BEGIN_SYS

void CPU::init()
{
    db<Init, CPU>(TRC) << "CPU::init()" << endl;

    _cpu_clock = System::info()->tm.cpu_clock;
    _cpu_current_clock = System::info()->tm.cpu_clock;
    _bus_clock = System::info()->tm.bus_clock;

    //Initialize the MMU
    if(CPU::id() == CPU::BSP) {
        if(Traits<MMU>::enabled)
            MMU::init();
        else
            db<Init, MMU>(WRN) << "MMU is disabled!" << endl;
    }

    // Initialize the PMU	
    /*
    PMU CHANNEL  || EVENT:
    ============ || ===================
    0            || LLC_REFERENCES              = 20270;
    1            || LLC_MISSES                  = 16686;
    2            || BRANCH_INSTRUCTIONS_RETIRED = 196;
    3            || BRANCH_MISSES_RETIRED       = 197;
    4            || UNHALTED_CORE_CYCLES        = 60;
    */
    if(Traits<PMU>::enabled) {
        PMU::init();
        //PMU::config(4, 316); //UNHALTED_REFERENCE_CYCLES
        // PMU::config(4, 60); //UNHALTED_CORE_CYCLES
        
        // EVENT deve ser menor que 233. --> ia32_pmu.h linha 635
        PMU::config(6, 15); // BRANCH_MISSES_RETIRED
        PMU::config(5, 11); // BRANCH_INSTRUCTIONS_RETIRED
        PMU::config(4, 30); // LLC_MISSES
        PMU::config(3, 29); // LLC_REFERENCES
        PMU::config(2, 2);  // INSTRUCTIONS_RETIRED
        PMU::config(1, 1);  // UNHALTED_CYCLES
        PMU::config(0, 0);  // CPU_CYCLES
        PMU::start(6);
        PMU::start(5);
        PMU::start(4);
        PMU::start(3);
        PMU::start(2);
        PMU::start(1);
        PMU::start(0);
        PMU::reset(6);
        PMU::reset(5);
        PMU::reset(4);
        PMU::reset(3);
        PMU::reset(2);
        PMU::reset(1);
        PMU::reset(0);
    }

}

void CPU::smp_barrier_init(unsigned int cores) {
    // Core activation in IA32 is handled by the APIC
    _cores = cores;
    if(multicore)
        APIC::remap();
};

__END_SYS
