// EPOS Clerk Implementation

#include <clerk.h>

__BEGIN_SYS

// Clerk
#ifdef __PMU_H

bool Clerk<PMU>::_in_use[Traits<Build>::CPUS][CHANNELS];
constexpr CPU::Reg32 ARMv8_A_PMU::_events[PMU::EVENTS];

#endif

// System_Monitor
Simple_List<Monitor> Monitor::_monitors[Traits<Build>::CPUS];
FANN_EPOS::fann * Monitor::ann[Traits<Build>::CPUS];
//unsigned int Monitor::ann_out[Traits<Build>::CPUS][100*30];
unsigned int Monitor::ann_captures[Traits<Build>::CPUS];
bool Monitor::_enable;

void Monitor::run()
{
    if(_enable) { 
        db<Monitor>(TRC) << "Monitor::run()" << endl;
        Simple_List<Monitor> * monitor = &_monitors[CPU::id()];
        for(List::Iterator it = monitor->begin(); it != monitor->end(); it++)
            it->object()->capture();
    }
}

void Monitor::init()
{
    if(!CPU::id()) {
        db<Monitor>(WRN) << "Monitor::init()" << TOTAL_EVENTS_MONITORED << endl;
        for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i)
        {
            ann[i] = FANN_EPOS::fann_create_from_config();
            db<Monitor>(WRN) << "ANN on = " << ann[i] << endl;
            ann_captures[i] = 0;
        }
    }


#ifdef __PMU_H

    if(Traits<PMU>::enabled && Traits<PMU>::monitored)
        init_pmu_monitoring<0>();

#endif

    if(Traits<System>::monitored)
        init_system_monitoring<0>();

}

__END_SYS
