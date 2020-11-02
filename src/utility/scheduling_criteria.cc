// EPOS CPU Affinity Scheduler Component Implementation

#include <utility/scheduler.h>
#include <time.h>
#include <clerk.h>
#include <process.h>

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

    bool Priority::collect() {
        return false;
    }

    
    bool Priority::charge() {
        return false;
    }

    // check ANN result and actuate
    // each cpu stores a result buffer, only one runs voting and clears everything
    bool Priority::award(bool hyperperiod) {//unsigned int cpu) {
        return false;
    }

    bool PEDF::collect() {
        if (learning){
            unsigned int cpu = CPU::id();
            Thread *t;
            float pre_mean;
            for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[cpu]; i++)
            {
                t = Thread::_Statistics::threads_cpu[cpu][i];
                if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                    // Collect all threads data
                    for (unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); ++j)
                    {
                        t->_statistics.input[j] = (t->_statistics.thread_pmu_accumulated[j]*1.0) /(Thread::_Statistics::hyperperiod[1]/100.);
                        t->_statistics.thread_monitoring[j][Thread::_Statistics::hyperperiod_count[cpu]] = t->_statistics.input[j];
                        pre_mean = t->_statistics.mean[j];
                        // mean[n] = mean[n-1] + (x - mean[n-1])/n
                        t->_statistics.mean[j] = pre_mean + (t->_statistics.input[j] - pre_mean)/(t->_statistics.captures+1);
                        // s[n] = s[n-1] + (x - mean[n-1])*(x - mean[n])
                        t->_statistics.variance[j] = t->_statistics.variance[j] + (t->_statistics.input[j] - pre_mean)*(t->_statistics.input[j] - t->_statistics.mean[j]);
                        t->_statistics.thread_pmu_accumulated[j] = 0;
                    }
                    t->_statistics.captures++;
                    t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)+0]
                            = t->_statistics.jobs ? ((t->_statistics.average_execution_time*1.0)/Thread::_Statistics::hyperperiod[1])
                            : ((t->_statistics.execution_time*1.0)/Thread::_Statistics::hyperperiod[1]);
                    //t->_statistics.thread_monitoring[COUNTOF(Traits<Monitor>::PMU_EVENTS)+0][Thread::_Statistics::hyperperiod_count[cpu]] = t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)+0] * 10000;
                    t->_statistics.thread_monitoring[COUNTOF(Traits<Monitor>::PMU_EVENTS)+0][Thread::_Statistics::hyperperiod_count[cpu]] = t->_statistics.jobs ? (((t->_statistics.average_execution_time)*1.0) /Thread::_Statistics::hyperperiod[1])*10000
                            : ((t->_statistics.execution_time*1.0)/Thread::_Statistics::hyperperiod[1])*10000;
                    t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)+1] = Machine::frequency();
                    t->_statistics.thread_monitoring[COUNTOF(Traits<Monitor>::PMU_EVENTS)+1][Thread::_Statistics::hyperperiod_count[cpu]] = t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)+1];
                    t->_statistics.average_execution_time = 0;
                    t->_statistics.jobs = 0;
                }
            }
            return true;
        }
        return false;
    }

    bool PEDF::charge() {
        if (learning) {
            unsigned int cpu = CPU::id();
            if (!cpu)
                return true;
            if (collect()) {
                Thread *t;
                for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[cpu]; i++)
                {
                    t = Thread::_Statistics::threads_cpu[cpu][i];
                    if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                        t->_statistics.output = *FANN_EPOS::fann_run(Monitor::ann[cpu], t->_statistics.input, true);
                        t->_statistics.thread_monitoring[COUNTOF(Traits<Monitor>::PMU_EVENTS)+2][Thread::_Statistics::hyperperiod_count[cpu]] = (int) (t->_statistics.output*10000);
                        db<Thread>(TRC) << "<"
                            << t->_statistics.input[0] << ","
                            << t->_statistics.input[1] << ","
                            << t->_statistics.input[2] << ","
                            << t->_statistics.input[3] << ","
                            << t->_statistics.input[4] << ","
                            << t->_statistics.input[5] << ","
                            << t->_statistics.input[6] << ","
                            << t->_statistics.input[7] << "," 
                            << "out: " << t->_statistics.output << ">" << endl;
                    }
                }
                return true;
            }
        }
        return false;
    }

    bool PEDF::award(bool hyperperiod) {
        static bool imbalanced = false;
        static unsigned int last_max_swap_from;
        static unsigned int last_freq = 1200000000;
        static unsigned int last_max_from;
        static unsigned int last_max_to;
        static float migration_upgrade_threshold = 0.05;
        static unsigned long long imbalance_threshold = 0;
        //static float last_usage = 0;
        //static float last_usage_swap = 0;
        static float lr = 0.7;

        unsigned int cpu = CPU::id();
        if (learning) {
            if (Thread::_Statistics::to_learn[cpu]) {// last hyperperiod was decrease
                if (Thread::_Statistics::cooldown[cpu]) {
                    Thread *t;
                    for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[cpu]; i++)
                    {
                        t = Thread::_Statistics::threads_cpu[cpu][i];
                        if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                            t->_statistics.average_execution_time = 0;
                            t->_statistics.jobs = 0;
                            for(unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); j++) {
                                t->_statistics.thread_pmu_accumulated[j] = 0;
                                // mean[n] = mean[n-1] + (x - mean[n-1])/n
                                t->_statistics.mean[j] = 0;
                                // s[n] = s[n-1] + (x - mean[n-1])*(x - mean[n])
                                t->_statistics.variance[j] = 0;
                                t->_statistics.last_activity.vector[j] = 0;
                            }
                            t->_statistics.captures = 0;
                        }
                        
                    }
                    Thread::_Statistics::cooldown[cpu] = false;
                    return false;
                }
                float aux = 0;
                unsigned int train_count = 0;
                const unsigned int train_limit = 16;
                FANN_EPOS::fann_type *trains[train_limit];
                FANN_EPOS::fann_type desired_output[train_limit];

                Thread *t;
                float pre_mean;
                float value;
                for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[cpu]; i++)
                {
                    t = Thread::_Statistics::threads_cpu[cpu][i];
                    if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                        aux = t->_statistics.jobs ? ((t->_statistics.average_execution_time*1.0)/Thread::_Statistics::hyperperiod[1])
                                : t->_statistics.execution_time/Thread::_Statistics::hyperperiod[1];
                        //aux = t->_statistics.jobs ? ((t->_statistics.average_execution_time*1.0)/t->_statistics.jobs)/t->_statistics.period
                        //        : t->_statistics.execution_time/t->_statistics.period;
                        desired_output[train_count] = aux;
                        trains[train_count] = t->_statistics.input;
                        train_count++;
                        t->_statistics.average_execution_time = 0; //as collect isn't going to execute, we need to clear them here.
                        t->_statistics.jobs = 0;
                        for(unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); j++) {
                            pre_mean = t->_statistics.mean[j];
                            value = (t->_statistics.thread_pmu_accumulated[j]*1.0)/(Thread::_Statistics::hyperperiod[1]/100.);
                            // mean[n] = mean[n-1] + (x - mean[n-1])/n
                            t->_statistics.mean[j] = t->_statistics.mean[j] + (value - pre_mean)/(t->_statistics.captures+1);
                            // s[n] = s[n-1] + (x - mean[n-1])*(x - mean[n])
                            t->_statistics.variance[j] = t->_statistics.variance[j] + (value - pre_mean)*(value - t->_statistics.mean[j]);
                            t->_statistics.captures++;
                            t->_statistics.thread_pmu_accumulated[j] = 0;
                            t->_statistics.last_activity.vector[j] = 0;
                        }
                    }
                }
                if (train_count > 0) {
                    FANN_EPOS::fann_reset_MSE(Monitor::ann[cpu]);
                    float error = 0;
                    int max_train = Traits<PEDF>::MAX_TRAINS;
                    bool end = false;
                    int count = 0;
                    while(!end && max_train > 0) {
                        end = true;
                        for (unsigned int i = 0; i < train_count; ++i)
                        {
                            error = FANN_EPOS::fann_train_data_incremental(Monitor::ann[cpu], trains[i], &desired_output[i]);
                            if (error > Traits<PEDF>::TRAIN_MIN_ERROR) { // TODO traits
                                error = FANN_EPOS::fann_train_data_incremental(Monitor::ann[cpu], trains[i], &desired_output[i]);
                            }
                            end &= error <= Traits<PEDF>::TRAIN_MIN_ERROR;
                            Thread::_Statistics::trains_err[cpu][7 - ((int)(trains[i][7]*6+1))][count] = error;
                            count++;
                            max_train--;
                        }
                        error = FANN_EPOS::fann_train_data_incremental(Monitor::ann[cpu], trains[0], &desired_output[0]);
                        if (error > Traits<PEDF>::TRAIN_MIN_ERROR)
                            end = false;
                    }
                }
                Thread::_Statistics::to_learn[cpu] = false;
            } else if (Thread::_Statistics::cooldown[cpu]) {
                Thread *t;
                for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[cpu]; i++)
                {
                    t = Thread::_Statistics::threads_cpu[cpu][i];
                    if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                        t->_statistics.average_execution_time = 0;
                        t->_statistics.jobs = 0;
                        for(unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); j++) {
                            t->_statistics.thread_pmu_accumulated[j] = 0;
                            // mean[n] = mean[n-1] + (x - mean[n-1])/n
                            t->_statistics.mean[j] = 0;
                            // s[n] = s[n-1] + (x - mean[n-1])*(x - mean[n])
                            t->_statistics.variance[j] = 0;
                            t->_statistics.captures = 0;
                            t->_statistics.last_activity.vector[j] = 0;
                        }
                    }
                }
                Thread::_Statistics::cooldown[cpu] = false;
            } else {
                Thread::_Statistics::prediction_ready[cpu] = false;
                if(cpu && charge()) {
                    FANN_EPOS::fann_type usage = 0;
                    Thread *t;
                    bool error = false;
                    int max_variance_t = 0;
                    float aux = 0;
                    for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[cpu]; i++)
                    {
                        t = Thread::_Statistics::threads_cpu[cpu][i];
                        if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                            usage += t->_statistics.output;
                            for(unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); j++) {
                                aux = t->_statistics.variance[j]/(t->_statistics.mean[j]*t->_statistics.mean[j]);//Math::babylonian_sqrt(t->_statistics.variance[j]/t->_statistics.captures)/t->_statistics.mean[j];
                                if (aux > max_variance_t)
                                    max_variance_t = (int)aux;
                            }
                        }
                        if (t->_statistics.input[6] == 0) {
                            error = true;
                        }
                    }
                    float threshold = 0;
                    if (max_variance_t < Traits<Monitor>::VARIANCE_RANGES[0]) {
                        threshold = Traits<Monitor>::VARIANCE_THRESHOLDS[0]; // 0.05;
                    } else if (max_variance_t < Traits<Monitor>::VARIANCE_RANGES[1]) {
                        threshold = Traits<Monitor>::VARIANCE_THRESHOLDS[1]; // 0.1
                    } else {
                        threshold = Traits<Monitor>::VARIANCE_THRESHOLDS[2]; // 0.2
                    }
                    Thread::_Statistics::max_variance[cpu][Thread::_Statistics::hyperperiod_count[cpu]] = max_variance_t;

                    Thread::_Statistics::decrease_frequency[cpu] = usage < (1 - threshold);

                    return !error;
                    //return false;
                } else if (!cpu && Thread::_Statistics::prediction_ready[1] && Thread::_Statistics::prediction_ready[2] && Thread::_Statistics::prediction_ready[3]) {
                    // CPU0 orchestrate frequency downgrade 
                    bool vote = true;
                    float threshold = 0;
                    float max_variance_t = Thread::_Statistics::max_variance[1][Thread::_Statistics::hyperperiod_count[1]];
                    
                    if (max_variance_t < Thread::_Statistics::max_variance[2][Thread::_Statistics::hyperperiod_count[2]])
                        max_variance_t = Thread::_Statistics::max_variance[2][Thread::_Statistics::hyperperiod_count[2]];
                    
                    if (max_variance_t < Thread::_Statistics::max_variance[3][Thread::_Statistics::hyperperiod_count[3]])
                        max_variance_t = Thread::_Statistics::max_variance[3][Thread::_Statistics::hyperperiod_count[3]];

                    unsigned long long min = Thread::_Statistics::hyperperiod_idle_time[1] < Thread::_Statistics::hyperperiod_idle_time[2] ? 
                                                Thread::_Statistics::hyperperiod_idle_time[1] : Thread::_Statistics::hyperperiod_idle_time[2];
                    min = min < Thread::_Statistics::hyperperiod_idle_time[3] ? min : Thread::_Statistics::hyperperiod_idle_time[3];

                    unsigned long long max = Thread::_Statistics::hyperperiod_idle_time[1] > Thread::_Statistics::hyperperiod_idle_time[2] ? 
                                                Thread::_Statistics::hyperperiod_idle_time[1] : Thread::_Statistics::hyperperiod_idle_time[2];
                    max = max > Thread::_Statistics::hyperperiod_idle_time[3] ? max : Thread::_Statistics::hyperperiod_idle_time[3];

                    if (max_variance_t < Traits<Monitor>::VARIANCE_RANGES[0]) {
                        threshold = Traits<Monitor>::VARIANCE_THRESHOLDS[0]; // 0.05;
                    } else if (max_variance_t < Traits<Monitor>::VARIANCE_RANGES[1]) {
                        threshold = Traits<Monitor>::VARIANCE_THRESHOLDS[1]; // 0.1
                    } else {
                        threshold = Traits<Monitor>::VARIANCE_THRESHOLDS[2]; // 0.2
                    }

                    long long unsigned cap = (long long unsigned) (threshold * Thread::_Statistics::hyperperiod[1]);

                    if (Thread::_Statistics::hyperperiod_idle_time[1] < cap
                        || Thread::_Statistics::hyperperiod_idle_time[2] < cap
                        || Thread::_Statistics::hyperperiod_idle_time[3] < cap) {
                        Hertz freq = Machine::frequency();
                        if (freq < 1200000000) {
                            for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i) {
                                Thread::_Statistics::cooldown[i] = true;
                                Thread::_Statistics::prediction_ready[i] = false;
                            }
                            Machine::clock(freq + (100 * 1000 * 1000));
                            imbalanced = false;
                            imbalance_threshold = 0;
                            db<Thread>(WRN) << "withdraw f= "<< freq << ",cap=" << cap << ",icpu1=" << Thread::_Statistics::hyperperiod_idle_time[1] << ",icpu2=" << Thread::_Statistics::hyperperiod_idle_time[2] << ",icpu3=" << Thread::_Statistics::hyperperiod_idle_time[3] << endl;
                            return true;
                        }
                    }

                    for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i)
                    {
                        Thread::_Statistics::votes[i][Thread::_Statistics::hyperperiod_count[i]] = Thread::_Statistics::decrease_frequency[i];
                        Thread::_Statistics::idle_time_vote[i][Thread::_Statistics::hyperperiod_count[i]] = Thread::_Statistics::hyperperiod_idle_time[i];
                        vote &= Thread::_Statistics::decrease_frequency[i];
                        Thread::_Statistics::prediction_ready[i] = false;
                    }

                    if (vote) { // all CPUS decided for decrease
                        Hertz freq = Machine::frequency();
                        if (freq > 600000000) {
                            for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i) {
                                Thread::_Statistics::to_learn[i] = true;
                                Thread::_Statistics::cooldown[i] = true;
                            }
                            Machine::clock(freq - (100 * 1000 * 1000));
                        }
                    } else {
                        if (!imbalanced || max - min > imbalance_threshold) {
                            // Migration charging
                            float *weights = Thread::_Statistics::activity_weights;
                            Thread * t;
                            // update weights - disable activity weights update
                            if (false && last_max_from > 0) { // calc and learn weights
                                float error[4] = {0,0,0,0};

                                for (unsigned int c = 1; c < Traits<Build>::CPUS; ++c)
                                {
                                    Thread::_Statistics::activity_cpu[c].reset();
                                    //  calc for each cpu
                                    //      calc for each thread and compose
                                    for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[c]; i++)
                                    {
                                        t = Thread::_Statistics::threads_cpu[c][i];
                                        if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                                            for (unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); ++j)
                                            {
                                                t->_statistics.activity.vector[j] = (t->_statistics.input[j] * weights[j]) // input * weights
                                                                                    //* (Thread::_Statistics::hyperperiod[1]/100.) // how many slots fits in a hyperperiod
                                                                                    * (t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)]); // usage
                                                                                    // value * (utilization * hyp/slot) = ponderado por uso
                                            }
                                            t->_statistics.activity.usage = t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)];
                                            Thread::_Statistics::activity_cpu[c] += t->_statistics.activity;
                                        }
                                    }
                                    error[c] = Thread::_Statistics::activity_cpu[c].sum() - Thread::_Statistics::last_activity_cpu[c].sum();
                                }

                                for (unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); ++j) // gradient descent
                                {
                                    weights[j] -= (lr * 
                                        ((-Thread::_Statistics::last_activity_cpu[1].vector[j]*error[1] + -Thread::_Statistics::last_activity_cpu[2].vector[j]*error[2] + -Thread::_Statistics::last_activity_cpu[3].vector[j]*error[3])
                                        /(Traits<Build>::CPUS-1)));
                                }
                                db<Thread>(WRN) << "weights:" << weights[0] << "," << weights[1] << "," << weights[2]
                                                << "," << weights[3] << "," << weights[4] << "," << weights[5] << endl;
                            } 
                            // calc new vector
                            for (unsigned int c = 1; c < Traits<Build>::CPUS; ++c)
                            {
                                Thread::_Statistics::activity_cpu[c].reset();
                                //  calc for each cpu
                                //      calc for each thread and compose
                                for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[c]; i++)
                                {
                                    t = Thread::_Statistics::threads_cpu[c][i];
                                    if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                                        for (unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); ++j)
                                        {  
                                            t->_statistics.activity.vector[j] = (t->_statistics.input[j] * weights[j]) // input * weights
                                                                                //* (Thread::_Statistics::hyperperiod[1]/100.) // how many slots fits in a hyperperiod
                                                                                * (t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)]); // usage
                                                                                // value * (utilization * hyp/slot) = ponderado por uso
                                        }
                                        t->_statistics.activity.usage = t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)];
                                        Thread::_Statistics::activity_cpu[c] += t->_statistics.activity;
                                    }
                                }
                            }

                            //  find the migration that most increses variance and fits in scheduling slack
                            Thread::Activity *activity_cpu = Thread::_Statistics::activity_cpu;
                            //unsigned int aux[COUNTOF(Traits<Monitor>::PMU_EVENTS)];
                            float digest_avg = 0;
                            float digest_cpu[Traits<Build>::CPUS];

                            // variance of activity vector per CPU
                            for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i)
                            {
                                Thread::_Statistics::prediction_ready[i] = false;
                                digest_cpu[i] = activity_cpu[i].sum();
                                digest_avg += digest_cpu[i];
                            }
                            digest_avg /= Traits<Build>::CPUS - 1;

                            // variance^2 calculation (metric)
                            float variance = 0;
                            for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i)
                            {
                                if (digest_avg > digest_cpu[i]) {
                                    variance += (digest_avg - digest_cpu[i]) * (digest_avg - digest_cpu[i]);
                                } else {
                                    variance += (digest_cpu[i] - digest_avg) * (digest_cpu[i] - digest_avg);
                                }
                            }
                            variance /= Traits<Build>::CPUS - 1;
                            float base_variance = variance;
                            Hertz freq = Machine::frequency();
                            if (freq == 600000000) {
                                imbalanced = true;
                                last_max_from = 0;
                                last_max_swap_from = 0;
                                last_max_to = 0;
                                min = Thread::_Statistics::hyperperiod_idle_time[1] < Thread::_Statistics::hyperperiod_idle_time[2] ? 
                                                            Thread::_Statistics::hyperperiod_idle_time[1] : Thread::_Statistics::hyperperiod_idle_time[2];
                                min = min < Thread::_Statistics::hyperperiod_idle_time[3] ? min : Thread::_Statistics::hyperperiod_idle_time[3];

                                max = Thread::_Statistics::hyperperiod_idle_time[1] > Thread::_Statistics::hyperperiod_idle_time[2] ? 
                                                            Thread::_Statistics::hyperperiod_idle_time[1] : Thread::_Statistics::hyperperiod_idle_time[2];
                                max = max > Thread::_Statistics::hyperperiod_idle_time[3] ? max : Thread::_Statistics::hyperperiod_idle_time[3];

                                imbalance_threshold = max - min;
                                db<Thread>(WRN) << "end - freq=" << freq << endl;
                                db<Thread>(WRN) << "icpu1=" << Thread::_Statistics::hyperperiod_idle_time[1] << ",icpu2=" << Thread::_Statistics::hyperperiod_idle_time[2] << ",icpu3=" << Thread::_Statistics::hyperperiod_idle_time[3] << endl;
                                return true;
                            }

                            db<Thread>(WRN) << "cpu1=" << digest_cpu[1] << ",cpu2=" << digest_cpu[2] << ",cpu3=" << digest_cpu[3] << ",avg=" << digest_avg << ",variance=" << variance << ",freq=" << freq << endl;

                            unsigned int max_from = 0;
                            unsigned int max_index = 0;
                            unsigned int max_to = 0;

                            //unsigned int h = 0;
                            float aux_avg = 0;
                            float curr_variance = 0;
                            //Thread::Activity activity_cpu[Traits<Build>::CPUS] = {activity_cpu[0],activity_cpu[1],activity_cpu[2],activity_cpu[3]};
                            
                            for (unsigned int cc = 1; cc < Traits<Build>::CPUS; ++cc)
                            {
                                //h = Thread::_Statistics::hyperperiod_count[cc] - 1;
                                for (unsigned int ct = 0; ct < Thread::_Statistics::t_count_cpu[cc]; ct++)
                                {
                                    t = Thread::_Statistics::threads_cpu[cc][ct];
                                    if (!((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)))
                                        continue; // idle

                                    activity_cpu[cc] -= t->_statistics.activity;

                                    for (unsigned int ca = 1; ca < Traits<Build>::CPUS; ++ca)
                                    {
                                        if (cc == ca)
                                            continue;
                                        db<Thread>(TRC) << "In=" << cc << "," << ct << "," << ca << endl;
                                        // check if fits the task
                                        if (t->_statistics.migration_locked[ca] || !activity_cpu[ca].fits(t->_statistics.activity))
                                            continue;

                                        // update avg with new cc
                                        aux_avg = (digest_avg*(Traits<Build>::CPUS-1) + activity_cpu[cc].sum()) - digest_cpu[cc];
                                        curr_variance = 0;

                                        db<Thread>(TRC) << "continue=" << cc << "," << ct << "," << ca << endl;
                                        activity_cpu[ca] += t->_statistics.activity;

                                        // update avg with new ca
                                        aux_avg += activity_cpu[ca].sum();
                                        aux_avg -= digest_cpu[ca];
                                        aux_avg /= Traits<Build>::CPUS - 1;

                                        db<Thread>(TRC) << "cpuca" << ca <<"=" << activity_cpu[ca].sum() << ",t=" << ct << ",avg=" << aux_avg << endl;

                                        // variance^2 calculation (metric)
                                        for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i) {
                                            if (aux_avg > activity_cpu[i].sum())
                                                curr_variance += (aux_avg - activity_cpu[i].sum())*(aux_avg - activity_cpu[i].sum());
                                            else
                                                curr_variance += (activity_cpu[i].sum() - aux_avg)*(activity_cpu[i].sum() - aux_avg);
                                        }
                                        curr_variance /= Traits<Build>::CPUS - 1;
                                        db<Thread>(TRC) << "variance=" << curr_variance << endl;
                                        if (curr_variance < variance && curr_variance < base_variance * (1 - migration_upgrade_threshold)) {
                                            variance = curr_variance;
                                            max_from = cc;
                                            max_index = ct;
                                            max_to = ca;
                                        }
                                        db<Thread>(TRC) << "cc=" << Thread::_Statistics::hyperperiod_idle_time[cc] << ",ca=" << Thread::_Statistics::hyperperiod_idle_time[ca] << ",t=" <<  t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)] << ",var=" << curr_variance
                                        << ",avg=" << aux_avg << ",c1=" << activity_cpu[1].sum()
                                                                << ",c2=" << activity_cpu[2].sum()
                                                                << ",c3=" << activity_cpu[3].sum() << endl;
                                        activity_cpu[ca] -= t->_statistics.activity;
                                    }
                                    activity_cpu[cc] += t->_statistics.activity;
                                }
                            }

                            db<Thread>(WRN) << "from=" << max_from << ",thread=" << max_index << ",to=" << max_to << ",var=" << variance << endl;// ",variance=" << variance << endl;
                            db<Thread>(WRN) << "icpu1=" << Thread::_Statistics::hyperperiod_idle_time[1] << ",icpu2=" << Thread::_Statistics::hyperperiod_idle_time[2] << ",icpu3=" << Thread::_Statistics::hyperperiod_idle_time[3] << endl;

                            if ((last_freq < freq && last_max_from > 0 && (max_from == 0 || last_max_swap_from == 0 || ((max_to != last_max_from && max_from != last_max_to) && last_max_swap_from > 0)))|| //) {// ||
                                (last_freq == freq && (max_to == last_max_from && max_from == last_max_to && last_max_swap_from == 0 && max_from != 0))) { // update weigths TODO
                                // worsen frequency with no swap, undo
                                // worsen frequency with a swap, if swap members are not involved, undo
                                // same frequnecy already undoing last migration
                                // worsen frequency through a simple migration (enters if worsen freq, not first round and no further migration or next migration is a simple undo of last migration (swap is ok to undo one part))
                                if (last_max_swap_from > 0) {
                                    // in swap: last_from == swto and last_to == swfrom
                                    // save in aux last from and set migrate
                                    Thread * t_aux = Thread::_Statistics::threads_cpu[last_max_from][Thread::_Statistics::t_count_cpu[last_max_from]-1];
                                    t_aux->_statistics.migrate_to = last_max_to;

                                    //if (t_aux->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)] > (last_usage_swap * 1.1))
                                        t_aux->_statistics.migration_locked[last_max_from] = true;

                                    //if (Thread::_Statistics::threads_cpu[last_max_to][Thread::_Statistics::t_count_cpu[last_max_to]-1]->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)]
                                    //    > (last_usage*1.1))
                                        Thread::_Statistics::threads_cpu[last_max_to][Thread::_Statistics::t_count_cpu[last_max_to]-1]->_statistics.migration_locked[last_max_to] = true;

                                    // last from = last to (reverse of to = from)
                                    Thread::_Statistics::threads_cpu[last_max_from][Thread::_Statistics::t_count_cpu[last_max_from]-1] 
                                        = Thread::_Statistics::threads_cpu[last_max_to][Thread::_Statistics::t_count_cpu[last_max_to]-1];
                                    
                                    // last to = aux (swfrom = swto, reverse of swto = swfrom)
                                    // last_to == swfrom
                                    Thread::_Statistics::threads_cpu[last_max_to][Thread::_Statistics::t_count_cpu[last_max_to]-1]->_statistics.migrate_to = last_max_from;
                                    Thread::_Statistics::threads_cpu[last_max_to][Thread::_Statistics::t_count_cpu[last_max_to]-1]
                                        = t_aux;
                                } else {
                                    // no swap, just reverse to = from (do last_from = last_to), then fix sizes
                                    //if (Thread::_Statistics::threads_cpu[last_max_to][Thread::_Statistics::t_count_cpu[last_max_to]-1]->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)]
                                    //    > (last_usage*1.1))
                                        Thread::_Statistics::threads_cpu[last_max_to][Thread::_Statistics::t_count_cpu[last_max_to]-1]->_statistics.migration_locked[last_max_to] = true;
                                    Thread::_Statistics::threads_cpu[last_max_to][Thread::_Statistics::t_count_cpu[last_max_to]-1]->_statistics.migrate_to = last_max_from;
                                    Thread::_Statistics::threads_cpu[last_max_from][Thread::_Statistics::t_count_cpu[last_max_from]] = Thread::_Statistics::threads_cpu[last_max_to][Thread::_Statistics::t_count_cpu[last_max_to]-1];
                                    Thread::_Statistics::t_count_cpu[last_max_from]++;
                                    Thread::_Statistics::t_count_cpu[last_max_to]--;
                                }
                                migration_upgrade_threshold += 0.15;
                                Thread::_Statistics::cooldown[last_max_from] = true;
                                Thread::_Statistics::cooldown[last_max_to] = true;
                                last_max_swap_from = 0;
                                last_max_from = 0;
                                last_max_to = 0;
                                //last_usage = 0;
                                //last_usage_swap = 0;
                                db<Thread>(WRN) << "undo migration" << endl;
                                //Machine::clock(last_freq); //go back to last clock
                                Machine::clock(1200000000);
                                //imbalanced = true;
                                return true;
                            }

                            unsigned int max_swap_from = 0;
                            unsigned int max_swap_index = 0;
                            unsigned int max_swap_to = 0;
                            if (max_from == 0){
                                // try swap now
                                Thread *ta;
                                for (unsigned int cc = 1; cc < Traits<Build>::CPUS-1; ++cc)
                                {
                                    //h = Thread::_Statistics::hyperperiod_count[cc] - 1;
                                    for (unsigned int ct = 0; ct < Thread::_Statistics::t_count_cpu[cc]; ct++)
                                    {
                                        t = Thread::_Statistics::threads_cpu[cc][ct];
                                        if (!((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)))
                                            continue; // idle

                                        activity_cpu[cc] -= t->_statistics.activity;

                                        for (unsigned int ca = cc+1; ca < Traits<Build>::CPUS; ++ca)
                                        {
                                            if (cc == ca)
                                                continue;
                                            for (unsigned int cta = 0; cta < Thread::_Statistics::t_count_cpu[ca]; cta++)
                                            {
                                                ta = Thread::_Statistics::threads_cpu[ca][cta];
                                                if (!((ta->priority() > PEDF::PERIODIC) && (ta->priority() < PEDF::APERIODIC)))
                                                    continue;

                                                db<Thread>(TRC) << "MOTHERFUCKER=" << cc << "," << ct << "," << ca << "," << cta << endl;
                                                activity_cpu[ca] -= ta->_statistics.activity;
                                                // check if fits the swap
                                                if (t->_statistics.migration_locked[ca] || ta->_statistics.migration_locked[cc] || !activity_cpu[ca].fits(t->_statistics.activity) ||
                                                    !activity_cpu[cc].fits(ta->_statistics.activity)) {
                                                    activity_cpu[ca] += ta->_statistics.activity;
                                                    continue;
                                                }
                                                db<Thread>(TRC) << "continue=" << cc << "," << ct << "," << ca << "," << cta<< endl;

                                                activity_cpu[ca] += t->_statistics.activity;
                                                activity_cpu[cc] += ta->_statistics.activity;

                                                // update avg with new cc
                                                aux_avg = (digest_avg*(Traits<Build>::CPUS-1) + activity_cpu[cc].sum()) - digest_cpu[cc];
                                                aux_avg += activity_cpu[ca].sum();
                                                aux_avg -= digest_cpu[ca];
                                                aux_avg /= Traits<Build>::CPUS - 1;
                                                curr_variance = 0;
                                                db<Thread>(TRC) << "cpuca" << ca <<"=" << activity_cpu[ca].sum() << ",t=" << ct << "," << cta << ",avg=" << aux_avg << endl;

                                                // variance^2 calculation (metric)
                                                for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i) {
                                                    if (aux_avg > activity_cpu[i].sum())
                                                        curr_variance += (aux_avg - activity_cpu[i].sum())*(aux_avg - activity_cpu[i].sum());
                                                    else
                                                        curr_variance += (activity_cpu[i].sum() - aux_avg)*(activity_cpu[i].sum() - aux_avg);
                                                }
                                                curr_variance /= Traits<Build>::CPUS - 1;
                                                db<Thread>(TRC) << "variance=" << curr_variance << endl;

                                                if (curr_variance < variance && curr_variance < base_variance * (1-migration_upgrade_threshold)) {
                                                    variance = curr_variance;
                                                    max_from = cc;
                                                    max_index = ct;
                                                    max_to = ca;
                                                    max_swap_from = ca;
                                                    max_swap_index = cta;
                                                    max_swap_to = cc;
                                                }
                                                db<Thread>(TRC) << "cc=" << Thread::_Statistics::hyperperiod_idle_time[cc] << ",ca=" << Thread::_Statistics::hyperperiod_idle_time[ca] << ",t=" <<  t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)] << ",var=" << curr_variance
                                                << ",avg=" << aux_avg << ",c1=" << activity_cpu[1].sum() << ",c2=" << activity_cpu[2].sum() << ",c3=" << activity_cpu[3].sum() << endl;
                                                activity_cpu[ca] -= t->_statistics.activity;
                                                activity_cpu[ca] += ta->_statistics.activity;
                                                activity_cpu[cc] -= ta->_statistics.activity;
                                            }
                                        }
                                        activity_cpu[cc] += t->_statistics.activity;
                                    }
                                }
                                db<Thread>(WRN) << "from=" << max_from << ",thread=" << max_index << ",to=" << max_to << ",var=" << variance << endl;// ",variance=" << variance << endl;
                                db<Thread>(WRN) << "swfrom=" << max_swap_from << ",swthread=" << max_swap_index << ",swto=" << max_swap_to << endl;// ",variance=" << variance << endl;
                                db<Thread>(WRN) << "icpu1=" << Thread::_Statistics::hyperperiod_idle_time[1] << ",icpu2=" << Thread::_Statistics::hyperperiod_idle_time[2] << ",icpu3=" << Thread::_Statistics::hyperperiod_idle_time[3] << endl;
                            }
                            last_max_swap_from = max_swap_from;
                            if (last_freq == 0 || freq < last_freq)
                                last_freq = freq;

                            last_max_from = max_from;
                            last_max_to = max_to;
                            if(max_from == 0) {
                                //last_usage = 0;
                                //last_usage_swap = 0;
                                db<Thread>(WRN) << "end - freq=" << freq << endl;
                                db<Thread>(WRN) << "icpu1=" << Thread::_Statistics::hyperperiod_idle_time[1] << ",icpu2=" << Thread::_Statistics::hyperperiod_idle_time[2] << ",icpu3=" << Thread::_Statistics::hyperperiod_idle_time[3] << endl;
                                min = Thread::_Statistics::hyperperiod_idle_time[1] < Thread::_Statistics::hyperperiod_idle_time[2] ? 
                                                            Thread::_Statistics::hyperperiod_idle_time[1] : Thread::_Statistics::hyperperiod_idle_time[2];
                                min = min < Thread::_Statistics::hyperperiod_idle_time[3] ? min : Thread::_Statistics::hyperperiod_idle_time[3];

                                max = Thread::_Statistics::hyperperiod_idle_time[1] > Thread::_Statistics::hyperperiod_idle_time[2] ? 
                                                            Thread::_Statistics::hyperperiod_idle_time[1] : Thread::_Statistics::hyperperiod_idle_time[2];
                                max = max > Thread::_Statistics::hyperperiod_idle_time[3] ? max : Thread::_Statistics::hyperperiod_idle_time[3];

                                imbalance_threshold = max - min;
                                last_max_from = 0;
                                last_max_swap_from = 0;
                                last_max_to = 0;

                                imbalanced = true;
                            } else {
                                //last_usage = Thread::_Statistics::threads_cpu[max_from][max_index]->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)];
                                Thread::_Statistics::activity_cpu[max_from] -= Thread::_Statistics::threads_cpu[max_from][max_index]->_statistics.activity;
                                Thread::_Statistics::activity_cpu[max_to] += Thread::_Statistics::threads_cpu[max_from][max_index]->_statistics.activity;
                                Thread::_Statistics::threads_cpu[max_from][max_index]->_statistics.migrate_to = max_to;
                                Thread::_Statistics::threads_cpu[max_to][Thread::_Statistics::t_count_cpu[max_to]] = Thread::_Statistics::threads_cpu[max_from][max_index];
                                Thread::_Statistics::t_count_cpu[max_to]++;
                                for (unsigned int i = max_index+1; i < Thread::_Statistics::t_count_cpu[max_from]; ++i)
                                {
                                    Thread::_Statistics::threads_cpu[max_from][i-1] = Thread::_Statistics::threads_cpu[max_from][i];
                                }
                                Thread::_Statistics::t_count_cpu[max_from]--;
                                Thread::_Statistics::cooldown[max_from] = true;
                                Thread::_Statistics::cooldown[max_to] = true;
                                if (max_swap_from != 0) {
                                    //last_usage_swap = Thread::_Statistics::threads_cpu[max_swap_from][max_swap_index]->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)];
                                    Thread::_Statistics::activity_cpu[max_swap_from] -= Thread::_Statistics::threads_cpu[max_swap_from][max_swap_index]->_statistics.activity;
                                    Thread::_Statistics::activity_cpu[max_swap_to] += Thread::_Statistics::threads_cpu[max_swap_from][max_swap_index]->_statistics.activity;
                                    Thread::_Statistics::threads_cpu[max_swap_from][max_swap_index]->_statistics.migrate_to = max_swap_to;
                                    Thread::_Statistics::threads_cpu[max_swap_to][Thread::_Statistics::t_count_cpu[max_swap_to]] = Thread::_Statistics::threads_cpu[max_swap_from][max_swap_index];
                                    Thread::_Statistics::t_count_cpu[max_swap_to]++;
                                    for (unsigned int i = max_swap_index+1; i < Thread::_Statistics::t_count_cpu[max_swap_from]; ++i)
                                    {
                                        Thread::_Statistics::threads_cpu[max_swap_from][i-1] = Thread::_Statistics::threads_cpu[max_swap_from][i];
                                    }
                                    Thread::_Statistics::t_count_cpu[max_swap_from]--;
                                }
                                Thread::_Statistics::last_activity_cpu[1] = Thread::_Statistics::activity_cpu[1];
                                Thread::_Statistics::last_activity_cpu[2] = Thread::_Statistics::activity_cpu[2];
                                Thread::_Statistics::last_activity_cpu[3] = Thread::_Statistics::activity_cpu[3];
                                Machine::clock(1200000000);
                            }
                        }

                            //imbalanced = true;
                            /* TS2
                            unsigned int x = 3; // from
                            unsigned int y = 1; // thread // thread 4 - 1
                            unsigned int z = 1; // to
                            Thread::_Statistics::threads_cpu[x][y]->_statistics.migrate_to = z;
                            Thread::_Statistics::threads_cpu[z][Thread::_Statistics::t_count_cpu[z]] = Thread::_Statistics::threads_cpu[x][y];
                            Thread::_Statistics::t_count_cpu[z]++;
                            for (unsigned int i = y+1; i < Thread::_Statistics::t_count_cpu[x]; ++i)
                            {
                                Thread::_Statistics::threads_cpu[x][i-1] = Thread::_Statistics::threads_cpu[x][i];
                            }
                            Thread::_Statistics::t_count_cpu[x]--;
                            Thread::_Statistics::cooldown[x] = true;
                            Thread::_Statistics::cooldown[z] = true;
                            //*/
                            
                            /* TS4
                            Thread::_Statistics::migration_hyperperiod[0] = Thread::_Statistics::hyperperiod_count[1];
                            Thread::_Statistics::migration_hyperperiod[1] = Thread::_Statistics::hyperperiod_count[2];
                            Thread::_Statistics::migration_hyperperiod[2] = Thread::_Statistics::hyperperiod_count[3];
                            //db<Thread>(WRN) << "MIGRATION=" << Thread::_Statistics::hyperperiod_count[1] << "," << Thread::_Statistics::hyperperiod_count[2] << "," << Thread::_Statistics::hyperperiod_count[3] << endl;

                            unsigned int x = 2; //from
                            unsigned int y = 4; // thread
                            unsigned int z = 1; // to
                            //Thread::_Statistics::threads_cpu[x][y]->_statistics.migrate_to = z;
                            //Thread::_Statistics::threads_cpu[z][Thread::_Statistics::t_count_cpu[z]] = Thread::_Statistics::threads_cpu[x][y];
                            //Thread::_Statistics::t_count_cpu[z]++;
                            //for (unsigned int i = y+1; i < Thread::_Statistics::t_count_cpu[x]; ++i)
                            //{
                            //    Thread::_Statistics::threads_cpu[x][i-1] = Thread::_Statistics::threads_cpu[x][i];
                            //}
                            //Thread::_Statistics::t_count_cpu[x]--;
                            //Thread::_Statistics::cooldown[x] = true;
                            //Thread::_Statistics::cooldown[z] = true;

                            x = 3; // from
                            y = 1; // thread
                            z = 1; // to
                            Thread::_Statistics::threads_cpu[x][y]->_statistics.migrate_to = z;
                            Thread::_Statistics::threads_cpu[z][Thread::_Statistics::t_count_cpu[z]] = Thread::_Statistics::threads_cpu[x][y];
                            Thread::_Statistics::t_count_cpu[z]++;
                            for (unsigned int i = y+1; i < Thread::_Statistics::t_count_cpu[x]; ++i)
                            {
                                Thread::_Statistics::threads_cpu[x][i-1] = Thread::_Statistics::threads_cpu[x][i];
                            }
                            Thread::_Statistics::t_count_cpu[x]--;
                            Thread::_Statistics::cooldown[x] = true;
                            Thread::_Statistics::cooldown[z] = true;

                            x = 2; // from
                            y = 3; // thread
                            z = 3; // to
                            Thread::_Statistics::threads_cpu[x][y]->_statistics.migrate_to = z;
                            Thread::_Statistics::threads_cpu[z][Thread::_Statistics::t_count_cpu[z]] = Thread::_Statistics::threads_cpu[x][y];
                            Thread::_Statistics::t_count_cpu[z]++;
                            for (unsigned int i = y+1; i < Thread::_Statistics::t_count_cpu[x]; ++i)
                            {
                                Thread::_Statistics::threads_cpu[x][i-1] = Thread::_Statistics::threads_cpu[x][i];
                            }
                            Thread::_Statistics::t_count_cpu[x]--;
                            Thread::_Statistics::cooldown[x] = true;
                            Thread::_Statistics::cooldown[z] = true;
                            //*/

                            /* TS3
                            unsigned int x = 2; //from
                            unsigned int y = 1; // thread
                            unsigned int z = 1; // to
                            Thread::_Statistics::threads_cpu[x][y]->_statistics.migrate_to = z;
                            Thread::_Statistics::threads_cpu[z][Thread::_Statistics::t_count_cpu[z]] = Thread::_Statistics::threads_cpu[x][y];
                            Thread::_Statistics::t_count_cpu[z]++;
                            for (unsigned int i = y+1; i < Thread::_Statistics::t_count_cpu[x]; ++i)
                            {
                                Thread::_Statistics::threads_cpu[x][i-1] = Thread::_Statistics::threads_cpu[x][i];
                            }
                            Thread::_Statistics::t_count_cpu[x]--;
                            Thread::_Statistics::cooldown[x] = true;
                            Thread::_Statistics::cooldown[z] = true;

                            x = 3; // from
                            y = 3; // thread
                            z = 1; // to
                            Thread::_Statistics::threads_cpu[x][y]->_statistics.migrate_to = z;
                            Thread::_Statistics::threads_cpu[z][Thread::_Statistics::t_count_cpu[z]] = Thread::_Statistics::threads_cpu[x][y];
                            Thread::_Statistics::t_count_cpu[z]++;
                            for (unsigned int i = y+1; i < Thread::_Statistics::t_count_cpu[x]; ++i)
                            {
                                Thread::_Statistics::threads_cpu[x][i-1] = Thread::_Statistics::threads_cpu[x][i];
                            }
                            Thread::_Statistics::t_count_cpu[x]--;
                            Thread::_Statistics::cooldown[x] = true;
                            Thread::_Statistics::cooldown[z] = true;

                            x = 3; // from
                            y = 3; // thread // thread 4 - 1
                            z = 1; // to
                            Thread::_Statistics::threads_cpu[x][y]->_statistics.migrate_to = z;
                            Thread::_Statistics::threads_cpu[z][Thread::_Statistics::t_count_cpu[z]] = Thread::_Statistics::threads_cpu[x][y];
                            Thread::_Statistics::t_count_cpu[z]++;
                            for (unsigned int i = y+1; i < Thread::_Statistics::t_count_cpu[x]; ++i)
                            {
                                Thread::_Statistics::threads_cpu[x][i-1] = Thread::_Statistics::threads_cpu[x][i];
                            }
                            Thread::_Statistics::t_count_cpu[x]--;
                            Thread::_Statistics::cooldown[x] = true;
                            Thread::_Statistics::cooldown[z] = true;
                            //*/
                            //Machine::clock(1200000000);//freq - (100 * 1000 * 1000));
                            //Monitor::ann[1] = FANN_EPOS::fann_create_from_config();
                            //Monitor::ann[2] = FANN_EPOS::fann_create_from_config();
                            //Monitor::ann[3] = FANN_EPOS::fann_create_from_config();
                        //}
                        // check for imbalance
                        /*
                        Thread::_Statistics::threads_cpu[x][y]->_statistics.migrate_to = z;
                        
                        for (int i = y+1; i < Thread::_Statistics::t_count_cpu[x]; ++i)
                        {
                            Thread::_Statistics::threads_cpu[x][i-1] = Thread::_Statistics::threads_cpu[x][i];
                        }
                        Thread::_Statistics::threads_cpu[z][Thread::_Statistics::t_count_cpu[z]] = next;
                        Thread::_Statistics::t_count_cpu[z]++;
                        Thread::_Statistics::t_count_cpu[cpu]--;
                        Thread::_Statistics::cooldown[x] = true;
                        Thread::_Statistics::cooldown[z] = true;
                        if (!imbalanced && (greater_idle - smaller_idle > balance_threshold)) { // create imbalanced and balance_threshold
                            
                        }
                        */
                    }
                }
            }
        }
        return false;
    }

};

__END_UTIL
