// EPOS CPU Affinity Scheduler Component Implementation

#include <utility/scheduler.h>
#include <time.h>
#include <clerk.h>

__BEGIN_UTIL

// Class attributes
volatile unsigned int Scheduling_Criteria::Variable_Queue::_next_queue;

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
            Simple_List<Monitor> *monitor = &(Monitor::_monitors[cpu]);
            if (monitor->tail()->object()->captures() < Monitor::ann_captures[cpu]+1) // we need two captures to start
                return false;
            FANN_EPOS::fann_type aux_input[Monitor::TOTAL_EVENTS_MONITORED-1];
            for(Simple_List<Monitor>::Iterator it = monitor->begin(); it != monitor->end() && i < Monitor::TOTAL_EVENTS_MONITORED-1; it++) { // ignore ddlm
                aux_input[i] = it->object()->last_capture(i);
                if (aux_input[i] < 0){ // never happens
                    reset = true;
                    break;
                }
                i++;
            }
            if(!reset) {
                for(i = 0; i < Monitor::TOTAL_EVENTS_MONITORED-1; i++) {
                    input[i] = aux_input[i];
                }
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
                //Simple_List<Monitor> *monitor = &(Monitor::_monitors[cpu]);
                //unsigned long long next_time = monitor->tail()->object()->last_time_stamp();
                //if (monitor->tail()->object()->captures() > Monitor::ann_captures[cpu]+1) // we need two captures to start
                //    return false;
                FANN_EPOS::fann_type *input = Thread::_Statistics::ann_inputs[cpu][Thread::_Statistics::count_ann[cpu]];
                //new FANN_EPOS::fann_type[Monitor::TOTAL_EVENTS_MONITORED-1];
                //Monitor::ann_last_capture[cpu] = next_time;
                //Monitor::ann_ts[cpu][Monitor::ann_captures[cpu]] = next_time;
                if(colect(input, cpu)) {
                    FANN_EPOS::fann_type *out = FANN_EPOS::fann_run(Monitor::ann[cpu], input, true);
                    if (cpu == 2)
                        db<Thread>(TRC) << "<"
                        << input[0] << ","
                        << input[1] << ","
                        << input[2] << ","
                        << input[3] << ","
                        << input[4] << ","
                        << input[5] << ","
                        << input[6] << ","
                        << input[7] << ","
                        << input[8] << ">" 
                        << "out: " << out[0] << "," << out[1] << "," << out[2] << endl;
                    if (out[0] > out[1]) {
                        if(out[0] > out[2]) {
                            Monitor::ann_out[cpu][Monitor::ann_captures[cpu]] = 0;
                        } else {
                            Monitor::ann_out[cpu][Monitor::ann_captures[cpu]] = 2;
                        }
                    } else {
                        if(out[1] > out[2]) {
                            Monitor::ann_out[cpu][Monitor::ann_captures[cpu]] = 1;
                        } else {
                            Monitor::ann_out[cpu][Monitor::ann_captures[cpu]] = 2;
                        }
                    }
                    Thread::_Statistics::ann_outputs[cpu][Thread::_Statistics::count_ann[cpu]][0] = out[0];
                    Thread::_Statistics::ann_outputs[cpu][Thread::_Statistics::count_ann[cpu]][1] = out[1];
                    Thread::_Statistics::ann_outputs[cpu][Thread::_Statistics::count_ann[cpu]][2] = out[2];
                    Monitor::ann_captures[cpu]++;
                    Thread::_Statistics::count_ann[cpu] = (Thread::_Statistics::count_ann[cpu] + 1 ) % 50;
                    if (Thread::_Statistics::size_ann[cpu] < 50) {
                        Thread::_Statistics::size_ann[cpu]++;
                    }
                    return true;
                }
            //}
        }
        return false;
    }

    // check ANN result and actuate
    // each cpu stores a result buffer, only one runs voting and clears everything
    bool Priority::award(bool hyperperiod) {//unsigned int cpu) {
        unsigned int cpu = CPU::id();
        if (!cpu)
            return false;
        if (learning && Monitor::ann_captures[cpu] > 1) {
            //Thread * t = Thread::self();
            if (hyperperiod) {
                unsigned int idle = (Thread::_Statistics::hyperperiod_idle_time[cpu]*100) / Thread::_Statistics::hyperperiod[cpu];
                if (idle != 0) {
                    // for each CPU do learn and delete previous captures
                    FANN_EPOS::fann_type desired_output[3];
                    unsigned int hyperperiod_threshold_up = 16; // > 1 frequencies downgrade 
                    unsigned int ddlm = Thread::_Statistics::missed_deadlines[cpu];
                    ///* TODO LEARN
                    if (!ddlm && idle >= hyperperiod_threshold_up) { 
                        // if i have 16% or more of free time i can decrease and no ddlm
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
                    if(cpu == 2)
                        db<Thread>(WRN) << "Train, idle=" << idle << ",ddlm=" << ddlm << ",out=" << desired_output[0] << "," << desired_output[1] << "," << desired_output[2] << endl;
                    ///*
                    unsigned int train_count = 0;
                    const unsigned int train_limit = 5;
                    FANN_EPOS::fann_type *trains[train_limit];
                    for(unsigned int j = 0; j < Thread::_Statistics::size_ann[cpu]; j++) {
                        // IF OUTPUT IS CORRECT, IGNORE TRAIN
                        if (Thread::_Statistics::ann_outputs[cpu][j][0] > Thread::_Statistics::ann_outputs[cpu][j][1]) {
                            if (Thread::_Statistics::ann_outputs[cpu][j][0] > Thread::_Statistics::ann_outputs[cpu][j][2]) {
                                if (desired_output[0] == 1)
                                    continue;
                            } else {
                                if (desired_output[2] == 1)
                                    continue;
                            }
                        } else if (Thread::_Statistics::ann_outputs[cpu][j][1] > Thread::_Statistics::ann_outputs[cpu][j][2]) {
                            if (desired_output[1] == 1)
                                    continue;
                        } else {
                            if (desired_output[2] == 1)
                                    continue;
                        }
                        trains[train_count++] = Thread::_Statistics::ann_inputs[cpu][j];
                        if (train_count >= train_limit)
                            break;
                    }
                    if (train_count > 0) {
                        float error = FANN_EPOS::fann_train_data(Monitor::ann[cpu], trains, train_count, desired_output);
                        db<Thread>(WRN) << "Train error=" << error << endl;
                    }
                    //*/
                }
                Thread::_Statistics::count_ann[cpu] = 0;
                Thread::_Statistics::size_ann[cpu] = 0;
                // do voting
                if (cpu != 1) // can be CPU 0, needs to check if others reached a hyperperiod first
                    return false;

                bool vote = true;
                //unsigned int idle[3];
                //unsigned int smaller_idle = ((unsigned int) 0xffffffff);
                for (unsigned int i = 1; i < Traits<Build>::CPUS; i++) {
                    db<Thread>(WRN) << "voting cpu[" << i << "]=" << Thread::_Statistics::decrease_frequency[i] 
                    << ",idle=" << Thread::_Statistics::hyperperiod_idle_time[i] 
                    << ",caps=" << Thread::_Statistics::size_ann[i] << endl;
                    vote &= Thread::_Statistics::decrease_frequency[i]; 
                    //idle[i] = (Thread::_Statistics::hyperperiod_idle_time[i]*100) / Thread::_Statistics::hyperperiod[i];
                    //smaller_idle = smaller_idle > Thread::_Statistics::hyperperiod_idle_time[i] ? Thread::_Statistics::hyperperiod_idle_time[i] : smaller_idle;
                    //greater_idle = greater_idle < Thread::_Statistics::hyperperiod_idle_time[i] ? Thread::_Statistics::hyperperiod_idle_time[i] : greater_idle;
                }

                if (vote) { // all CPUS decided for decrease
                    for (unsigned int i = 1; i < Traits<Build>::CPUS; i++) { // Clear learn buffer
                        Thread::_Statistics::count_ann[i] = 0;
                        Thread::_Statistics::size_ann[i] = 0;
                    }
                    Hertz freq = Machine::frequency();
                    db<Thread>(WRN) << "Down:" << freq << endl;
                    if (freq > 60000000)
                        Machine::clock(freq - (100 * 1000 * 1000));
                } else {
                    // check for imbalance
                    //if (!imbalanced && (greater_idle - smaller_idle > balance_threshold)) { // create imbalanced and balance_threshold
                    //    imbalanced = balance_load(); // HOW TODO THIS? --> force the correct migrations
                    //}
                }
            } else { // not hyperperiod
                Hertz freq;
                switch(Monitor::ann_out[cpu][Monitor::ann_captures[cpu]-1]) {
                    case 2:  // increase frequency
                        freq = Machine::frequency();
                        if (freq < 1200000000)
                            db<Thread>(WRN) << "UP:" << Machine::clock(freq + (100 * 1000 * 1000)) << endl;
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
