// EPOS CPU Affinity Scheduler Component Implementation

#include <utility/scheduler.h>
#include <time.h>
#include <clerk.h>

__BEGIN_UTIL

// Class attributes
volatile unsigned int Scheduling_Criteria::Variable_Queue::_next_queue;
Simple_List<Monitor> *monitor;

// The following Scheduling Criteria depend on Alarm, which is not available at scheduler.h
namespace Scheduling_Criteria {
    FCFS::FCFS(int p): Priority((p == IDLE) ? IDLE : Alarm::elapsed()) {}

    EDF::EDF(const Microsecond & d, const Microsecond & p, const Microsecond & c, int): RT_Common(Alarm::ticks(d), Alarm::ticks(d), p, c) {}

    void EDF::update() {
        if((_priority > PERIODIC) && (_priority < APERIODIC))
            _priority = Alarm::elapsed() + _deadline;
    }

    bool Priority::colect(FANN_EPOS::fann_type *input, unsigned int cpu) {
        if (learning) {
            int i = 0;
            bool reset = false;
            for(Simple_List<Monitor>::Iterator it = monitor->begin(); it != monitor->end() && i < 5; it++) {
                input[i] = it->object()->last_capture();
                if (input[i] < 0){
                    reset = true;
                    break;
                }
                i++;
            }
            return !reset;
        } else {
            return false;
        }
    }

    void Priority::charge() {
        if (learning && CPU::id() == 0) {
            // Simple_List<Monitor> * monitor;
            for(unsigned int cpu = 0; cpu < CPU::cores(); cpu++) {
                monitor = &(Monitor::_monitors[cpu]);
                unsigned long long next_time = monitor->tail()->object()->last_time_stamp();
                if (monitor->tail()->object()->captures() <= Monitor::ann_captures[cpu])
                    continue;
                FANN_EPOS::fann_type *input = new FANN_EPOS::fann_type[5];
                Monitor::ann_last_capture[cpu] = next_time;
                Monitor::ann_ts[cpu][Monitor::ann_captures[cpu]] = next_time;
                if(colect(input, cpu)) {
                    FANN_EPOS::fann_type *out = FANN_EPOS::fann_run(Monitor::ann, input);
                    if (out[0] < 0.7)
                        Monitor::ann_out[cpu][Monitor::ann_captures[cpu]] = 0;
                    else
                        Monitor::ann_out[cpu][Monitor::ann_captures[cpu]] = 1;
                } else {
                    Monitor::ann_out[cpu][Monitor::ann_captures[cpu]] = 10;
                }
                Monitor::ann_captures[cpu]++;
            }
        }
    }

    bool Priority::award(int p, unsigned int cpu) {
        if (learning) {
            if(Monitor::ann_captures[cpu] >= 2) {
                return !(p != IDLE && p != MAIN && 
                    Monitor::ann_out[cpu][Monitor::ann_captures[cpu]-2] == Monitor::ann_out[cpu][Monitor::ann_captures[cpu]-1] &&
                    Monitor::ann_out[cpu][Monitor::ann_captures[cpu]-1] == 0);
            }
        }
        return true;
    }

};

__END_UTIL
