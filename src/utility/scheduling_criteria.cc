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
            for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[cpu]; i++)
            {
                t = Thread::_Statistics::threads_cpu[cpu][i];
                if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                    // Collect all threads data
                    for (unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); ++j)
                    {
                        t->_statistics.input[j] = (t->_statistics.thread_pmu_accumulated[j]*1.0) / (t->_statistics.period/100.);
                        t->_statistics.thread_pmu_accumulated[j] = 0;
                    }
                    t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)+0]
                            = t->_statistics.jobs ? ((t->_statistics.average_execution_time*1.0)/t->_statistics.jobs)/t->_statistics.period
                            : t->_statistics.execution_time/t->_statistics.period;
                    t->_statistics.input[COUNTOF(Traits<Monitor>::PMU_EVENTS)+1]
                            = Machine::frequency();
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
                        //if (cpu == 2) {
                            t->_statistics.output = *FANN_EPOS::fann_run(Monitor::ann[cpu], t->_statistics.input, true);
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
                        //}
                    }
                }
                return true;
            }
        }
        return false;
    }

    bool PEDF::award(bool hyperperiod) {
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
                        }
                    }
                    Thread::_Statistics::cooldown[cpu] = false;
                    return false;
                }
                // TODO learn
                float aux = 0;
                float diff = 0;
                unsigned int train_count = 0;
                const unsigned int train_limit = 16;
                FANN_EPOS::fann_type *trains[train_limit];
                FANN_EPOS::fann_type desired_output[train_limit];

                Thread *t;
                for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[cpu]; i++)
                {
                    t = Thread::_Statistics::threads_cpu[cpu][i];
                    if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                        aux = t->_statistics.jobs ? ((t->_statistics.average_execution_time*1.0)/t->_statistics.jobs)/t->_statistics.period
                                : t->_statistics.execution_time/t->_statistics.period;
                        desired_output[train_count] = aux;
                        diff = aux - t->_statistics.output;
                        if (diff < 0)
                            diff *= -1;
                        if (diff > .01) {
                            trains[train_count] = t->_statistics.input;
                            train_count++;
                        }
                        t->_statistics.average_execution_time = 0; //as collect isn't been run, we need to clear them here.
                        t->_statistics.jobs = 0;
                    }
                }
                if (train_count > 0) {
                    float error = 0;
                    unsigned int max_train = 4;
                    bool end = false;
                    while(!end && max_train) {
                        end = true;
                        for (unsigned int i = 0; i < train_count; ++i)
                        {
                            error = FANN_EPOS::fann_train_data_incremental(Monitor::ann[cpu], trains[i], &desired_output[i]);
                            end &= error <= 0.03;
                            //if (cpu == 2) {
                            //    db<Thread>(WRN) << "T "<< i << "=" << error << endl;
                            //}
                        }
                        max_train--;
                    }
                }
                Thread::_Statistics::to_learn[cpu] = false;
            } else {
                if(cpu && charge()) {
                    FANN_EPOS::fann_type usage = 0;
                    Thread *t;
                    for (unsigned int i = 0; i < Thread::_Statistics::t_count_cpu[cpu]; i++)
                    {
                        t = Thread::_Statistics::threads_cpu[cpu][i];
                        if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) { // !idle
                            usage += t->_statistics.output;
                        }
                    }

                    // margem para baixar pode ser um algoritmo estatistico para medir a variação do sistema
                    // variancia para margem, se o comportamento esta regular, redução mais agressiva.
                    // Beta, taxa de desconfiança da IA
                    // Soma tudo (overhead do SO ta incorporada nos contadores) < 1 - Beta_variancia (.95, .9, .8)
                    Thread::_Statistics::decrease_frequency[cpu] = usage < (1 - 0.1);

                    Thread::_Statistics::pred_ready[cpu] = true;
                    return false;
                } else if (!cpu && Thread::_Statistics::pred_ready[1] && Thread::_Statistics::pred_ready[2] && Thread::_Statistics::pred_ready[3]) {
                    // CPU0 orchestrate frequency downgrade 
                    bool vote = true;
                    for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i)
                    {
                        db<Thread>(WRN) << "voting cpu[" << i << "]=" << Thread::_Statistics::decrease_frequency[i] 
                            << ",idle=" << Thread::_Statistics::hyperperiod_idle_time[i] << endl;
                        vote &= Thread::_Statistics::decrease_frequency[i];
                        Thread::_Statistics::pred_ready[i] = false;
                    }

                    Thread *t;
                    for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i) {
                        db<Thread>(WRN) << "CPU" << i << endl;
                        for (unsigned int j = 0; j < Thread::_Statistics::t_count_cpu[i]; j++)
                        {
                            t = Thread::_Statistics::threads_cpu[i][j];
                            if ((t->priority() > PEDF::PERIODIC) && (t->priority() < PEDF::APERIODIC)) {
                                db<Thread>(WRN) << "T" << j << "=" << t->_statistics.output << endl;
                            }
                        }
                    }

                    if (vote) { // all CPUS decided for decrease
                        Hertz freq = Machine::frequency();
                        db<Thread>(WRN) << "Down:" << freq << endl;
                        if (freq > 600000000) {
                            for (unsigned int i = 1; i < Traits<Build>::CPUS; ++i) {
                                Thread::_Statistics::to_learn[i] = true;
                                Thread::_Statistics::cooldown[i] = true;
                            }
                            Machine::clock(freq - (100 * 1000 * 1000));
                        }
                    } else {
                        // check for imbalance
                        //if (!imbalanced && (greater_idle - smaller_idle > balance_threshold)) { // create imbalanced and balance_threshold
                        //    imbalanced = balance_load(); // HOW TODO THIS? --> force the correct migrations
                        //}
                    }
                }
            }
        }
        return false;
    }

};

__END_UTIL
