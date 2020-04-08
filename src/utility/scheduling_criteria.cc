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
            unsigned int i = 0;
            bool reset = false;
            for(Simple_List<Monitor>::Iterator it = monitor->begin(); it != monitor->end() && i < Monitor::TOTAL_EVENTS_MONITORED-1; it++) { // ignore ddlm
                input[i] = it->object()->last_capture();
                if (input[i] < 0){ // never happens
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

    // collect input and run ANN
    // return if done
    bool Priority::charge() {
        if (learning){ //&& CPU::id() == 0) {
            // Simple_List<Monitor> * monitor;
            //for(unsigned int cpu = 0; cpu < CPU::cores(); cpu++) {
                unsigned int cpu = CPU::id();
                if (cpu == 1)
                    db<Thread>(WRN) << "Charge" << endl;
                monitor = &(Monitor::_monitors[cpu]);
                //unsigned long long next_time = monitor->tail()->object()->last_time_stamp();
                if (monitor->tail()->object()->captures() > Monitor::ann_captures[cpu]+1) // we need two captures to start
                    return false;
                FANN_EPOS::fann_type *input = new FANN_EPOS::fann_type[Monitor::TOTAL_EVENTS_MONITORED-1];
                //Monitor::ann_last_capture[cpu] = next_time;
                //Monitor::ann_ts[cpu][Monitor::ann_captures[cpu]] = next_time;
                if(colect(input, cpu)) {
                    if (cpu == 1)
                        db<Thread>(WRN) << "FANN RUN" << endl;
                    FANN_EPOS::fann_type *out = FANN_EPOS::fann_run(Monitor::ann[cpu], input); // TODO check ANN per CPU
                    if (out[0] > out[1]) {
                        if(out[0] > out[2]) {
                            Monitor::ann_out[cpu] = 0;
                        } else {
                            Monitor::ann_out[cpu] = 2;
                        }
                    } else {
                        if(out[1] > out[2]) {
                            Monitor::ann_out[cpu] = 1;
                        } else {
                            Monitor::ann_out[cpu] = 2;
                        }
                    }
                    Monitor::ann_captures[cpu]++;
                    Thread * t = Thread::self();

                    // maintain a maximum of 20 captures per hyperperiod
                    if (t->_statistics.size_ann[cpu] == 20) {
                        delete t->_statistics.ann_inputs[cpu][t->_statistics.count_ann[cpu]];
                        delete t->_statistics.ann_outputs[cpu][t->_statistics.count_ann[cpu]];
                    }
                    t->_statistics.ann_inputs[cpu][t->_statistics.count_ann[cpu]] = input;
                    t->_statistics.ann_outputs[cpu][t->_statistics.count_ann[cpu]] = out;
                    t->_statistics.count_ann[cpu] = (t->_statistics.count_ann[cpu] + 1 ) % 20;
                    if (t->_statistics.size_ann[cpu] < 20) {
                        t->_statistics.size_ann[cpu]++;
                    }
                    return true;
                } else {
                    delete input; // error
                }
            //}
        }
        return false;
    }

    // check ANN result and actuate
    // each cpu stores a result buffer, only one runs voting and clears everything
    bool Priority::award(bool hyperperiod) {//unsigned int cpu) {
        unsigned int cpu = CPU::id();
        if (learning && Monitor::ann_captures[cpu] > 1) {
            Thread * t = Thread::self();
            if (hyperperiod) {
                // do voting
                if (cpu != 1) // can be CPU 0, needs to check if others reached a hyperperiod first
                    return false;

                bool vote = true;
                unsigned int greater_idle = 0;
                unsigned int smaller_idle = ((unsigned int) 0xffffffff);
                for (unsigned int i = 1; i < Traits<Build>::CPUS; i++) {
                    db<Thread>(WRN) << "voting cpu[" << i << "]=" << t->_statistics.decrease_frequency[i] << endl;
                    vote &= t->_statistics.decrease_frequency[i]; 
                    smaller_idle = smaller_idle > t->_statistics.hyperperiod_idle_time[i] ? t->_statistics.hyperperiod_idle_time[i] : smaller_idle;
                    greater_idle = greater_idle < t->_statistics.hyperperiod_idle_time[i] ? t->_statistics.hyperperiod_idle_time[i] : greater_idle;
                }

                if (vote) { // all CPUS decided for decrease
                    for (unsigned int i = 1; i < Traits<Build>::CPUS; i++) { // Clear learn buffer
                        for(unsigned int j = 0; j < t->_statistics.size_ann[i]; j++) {
                            delete t->_statistics.ann_inputs[i][j];
                            delete t->_statistics.ann_outputs[i][j];
                        }
                        t->_statistics.count_ann[i] = 0;
                        t->_statistics.size_ann[i] = 0;
                    }
                    Machine::clock(Machine::frequency() - (100 * 1000 * 1000));
                } else {
                    // for each CPU do learn and delete previous captures
                    // fann_type* desired_output[3];
                    for (unsigned int i = 1; i < Traits<Build>::CPUS; i++) {
                        /* // TODO LEARN
                        // monitor = &(Monitor::_monitors[i]);
                        // unsigned int ddlm = (monitor->begin() + Monitor::TOTAL_EVENTS_MONITORED-2)->object()->last_capture()
                        if (!ddlm && t->_statistics.hyperperiod_idle_time[i] >= hyperperiod_threshold_up) { 
                            // if i have 10% or more of free time i can decrease and no ddlm
                            desired_output[0] = 1;
                            desired_output[1] = -1;
                            desired_output[2] = -1;
                        } else if (!ddlm) { 
                            // if no deadline miss accounted i can maintain
                            desired_output[0] = -1;
                            desired_output[1] = 1;
                            desired_output[2] = -1;
                        } else {
                            // increase
                            desired_output[0] = -1;
                            desired_output[1] = -1;
                            desired_output[2] = 1;
                        }
                        //*/
                        for(unsigned int j = 0; j < t->_statistics.size_ann[i]; j++) {
                            /*
                            FANN_EPOS::fann_set_train_in_out(Monitor::ann[i], t->_statistics.ann_inputs[i][j], t->_statistics.ann_outputs[i][j]);
                            FANN_EPOS::fann_learn_last_run(Monitor::ann[i], desired_output);
                            //*/
                            delete t->_statistics.ann_inputs[i][j];
                            delete t->_statistics.ann_outputs[i][j];
                        }
                        t->_statistics.count_ann[i] = 0;
                        t->_statistics.size_ann[i] = 0;
                    }
                    // check for imbalance
                    //if (!imbalanced && (greater_idle - smaller_idle > balance_threshold)) { // create imbalanced and balance_threshold
                    //    imbalanced = balance_load(); // HOW TODO THIS? --> force the correct migrations
                    //}
                }
            } else { // not hyperperiod
                switch(Monitor::ann_out[cpu]) {
                    case 2:  // increase frequency
                        Machine::clock(Machine::frequency() + (100 * 1000 * 1000));
                        return false;
                    case 1:  // maintain frequency
                        return false;
                    default: // decrease frequency
                        return true;
                }
            }
        }
        return false;
    }

};

__END_UTIL
