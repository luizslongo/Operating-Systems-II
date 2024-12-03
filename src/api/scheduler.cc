// EPOS CPU Scheduler Component Implementation

#include "utility/wrapped_ostream.h"
#include <process.h>
#include <time.h>

__BEGIN_SYS

OStream cout;
volatile unsigned int Variable_Queue_Scheduler::_next_queue;

inline RT_Common::Tick RT_Common::elapsed() { return Alarm::elapsed(); }

RT_Common::Tick RT_Common::ticks(Microsecond time)
{
    return Timer_Common::ticks(time, Alarm::timer()->frequency());
}

Microsecond RT_Common::time(Tick ticks)
{
    return Timer_Common::time(ticks, Alarm::timer()->frequency());
}

void RT_Common::handle(Event event)
{
    db<Thread>(TRC) << "RT::handle(this=" << this << ",e=";
    if (event & CREATE)
    {
        db<Thread>(TRC) << "CREATE";

        _statistics.thread_creation = elapsed();
        _statistics.job_released = false;
    }
    if (event & FINISH)
    {
        db<Thread>(TRC) << "FINISH";

        _statistics.thread_destruction = elapsed();
    }
    if (event & ENTER)
    {
        db<Thread>(TRC) << "ENTER";

        _statistics.thread_last_dispatch = elapsed();
        _statistics.number_dispatches++;
    }
    if (event & LEAVE)
    {
        Tick cpu_time = elapsed() - _statistics.thread_last_dispatch;

        db<Thread>(TRC) << "LEAVE";

        _statistics.thread_last_preemption = elapsed();
        _statistics.thread_execution_time += cpu_time;
        _statistics.job_utilization += cpu_time;
        //        if(_statistics.job_released) {
        //        }
    }
    if (periodic() && (event & JOB_RELEASE))
    {
        db<Thread>(TRC) << "RELEASE";

        if (_statistics.jobs_released < 5)
            _statistics.average_job_execution_time += _statistics.job_utilization / 5;
        else
        {
            _statistics.average_job_execution_time = (_statistics.average_job_execution_time * 4 + _statistics.job_utilization) / 5;
        }

        int task_type = ((BEST_EFFORT & _priority) == BEST_EFFORT ? BEST_EFFORT : CRITICAL);
        int absolute_deadline = _priority - task_type;

        if (absolute_deadline < elapsed())
            _statistics.deadlines_missed++;


        if (Traits<EDF_Modified>::ENABLE_STATISTICS)
        {
            _ostream_lock.acquire();
            cout << '<' << CPU::id() << "> " << "JOB AVERAGE EXECUTION TIME (last 5 executions): " << _statistics.average_job_execution_time << '\n';
            cout << '<' << CPU::id() << "> " << "JOB UTILIZATION: " << _statistics.job_utilization << '\n';
            _ostream_lock.release();
        }
        _statistics.job_released = true;
        _statistics.job_release = elapsed();
        _statistics.job_start = 0;
        _statistics.job_utilization = 0;
        _statistics.jobs_released++;
    }
    if (periodic() && (event & JOB_FINISH))
    {
        db<Thread>(TRC) << "WAIT";

        _statistics.job_released = false;
        _statistics.job_finish = elapsed();
        _statistics.jobs_finished++;
       //        _statistics.job_utilization += elapsed() - _statistics.thread_last_dispatch;
    }
    if (event & COLLECT)
    {
        db<Thread>(TRC) << "|COLLECT";
    }
    if (periodic() && (event & CHARGE))
    {
        _statistics.num_charges++;
        db<Thread>(TRC) << "|CHARGE";
    }
    if (periodic() && (event & AWARD))
    {
        db<Thread>(TRC) << "|AWARD";
    }
    if (periodic() && (event & UPDATE))
    {
        db<Thread>(TRC) << "|UPDATE";
    }
    db<Thread>(TRC) << ") => {i=" << _priority << ",p=" << _period << ",d=" << _deadline << ",c=" << _capacity << "}" << endl;
}

EDF_Modified::EDF_Modified(Microsecond p, Microsecond d, Microsecond c, int task_type) : RT_Common(int(elapsed() + ticks(d)) | task_type, p, d, c), _min_frequency(CPU::max_clock()), _max_frequency(CPU::max_clock()), _last_deadline(elapsed()), _step(_max_frequency)
{
    memset(&_statistics, 0, sizeof(_statistics));
    // _statistics.thread_last_dispatch = _statistics.thread_last_preemption = _statistics.jobs_released = _statistics.job_utilization = _statistics.average_job_execution_time = 0;
}

void EDF_Modified::_calculate_min_frequency()
{
    if (_statistics.jobs_released < 5)
        return;

    unsigned int average_execution_fraction = (_statistics.average_job_execution_time * 10) / _deadline;


    if (Traits<EDF_Modified>::ENABLE_STATISTICS)
    {
        _ostream_lock.acquire();
        cout << '<' << CPU::id() << "> " << "AVERAGE EF: " << average_execution_fraction << ", _DEADLINE: " << _deadline << '\n';
        _ostream_lock.release();
    }

    if (_statistics.jobs_released == 5)
    {
        Hertz step = (CPU::max_clock() - CPU::min_clock()) / 10;
        _min_frequency = CPU::min_clock() + step * average_execution_fraction;
        return;
    }
    // fazemos dessa forma pra ter uma mudança mais previsível no min_cpu_frequency.
    // Pensei em fazer matematicamente, mas n achei nenhuma forma boa.
    switch (average_execution_fraction)
    {
    case 0:
    case 1:
        _min_frequency -= _min_frequency / 2; // diminui 25%
        break;                                // entre 0% e 20%
    case 2:
    case 3:
        _min_frequency -= _min_frequency / 2; // diminui 12.5%
        break;                                // entre 20% e 40%
    case 4:
    case 5:
    case 6:
        break; // entre 40% e 70%
    case 7:
    case 8:
        _min_frequency += _min_frequency / 8; // aumenta 12.5%
        break;                                // entre 70% e 90%
    default:
        _min_frequency += _min_frequency / 8; // aumenta 25%
        // acima de 90%
    }

    // Resolvendo problemas de contorno
    if (_min_frequency < CPU::min_clock())
        _min_frequency = CPU::min_clock();
    else if (_min_frequency > CPU::max_clock())
        _min_frequency = CPU::max_clock();
}

void EDF_Modified::_handle_charge(Event event)
{
    if (!periodic())
    {
        if (CPU::clock() != _max_frequency)
        {
            if (Traits<EDF_Modified>::ENABLE_STATISTICS)
            {
                _ostream_lock.acquire();
                cout << '<' << CPU::id() << "> " << "Aperiodic Task!!! Putting CPU on MAX FREQUENCY\n";
                _ostream_lock.release();
            }
            CPU::clock(_max_frequency);
        }
        // _statistics.average_frequency =  ((unsigned long long)_statistics.average_frequency * (_statistics.num_charges - 1) + _max_frequency)/_statistics.num_charges;
        return;
    }

    unsigned int task_type = ((BEST_EFFORT & _priority) == BEST_EFFORT ? BEST_EFFORT : CRITICAL);

    unsigned int absolute_deadline = _priority - task_type;
    unsigned int start_time = _last_deadline;
    unsigned int current_time = elapsed();

    if (current_time > absolute_deadline)
    {
        if (Traits<EDF_Modified>::ENABLE_DEADLINE_PRINT)
        {
            _ostream_lock.acquire();
            cout << '<' << CPU::id() << "> " << "\n========================================================\n";
            cout << '<' << CPU::id() << "> " << "Deadline Missed!!! Putting CPU on MAX FREQUENCY\n";
            cout << '<' << CPU::id() << "> " << "CURRENT: " << current_time << ", START: " << start_time << ", AB DL: " << absolute_deadline << "\n";
            cout << '<' << CPU::id() << "> " << "\n========================================================\n";
            _ostream_lock.release();
        }
        CPU::clock(_max_frequency);
        _statistics.average_frequency =  ((unsigned long long)_statistics.average_frequency * (_statistics.num_charges - 1) + _max_frequency)/_statistics.num_charges;
        return;
    }

    if (_statistics.number_dispatches % (task_type == BEST_EFFORT ? 10 : 3) != 0)
    {
        if (Traits<EDF_Modified>::ENABLE_STATISTICS)
        {
            _ostream_lock.acquire();
            cout << '<' << CPU::id() << "> " << "\n========================================================\n";
            cout << '<' << CPU::id() << "> " << "NUMBER OF DISPATCHES NOT ENOUGH TO RECALCULATE FREQUENCY\n";
            cout << '<' << CPU::id() << "> " << "TASK TYPE: " << (task_type == BEST_EFFORT ? "BEST_EFFORT" : "CRITICAL") << "\n";
            cout << '<' << CPU::id() << "> " << "NUMBER DISPATCHES = " << _statistics.number_dispatches << "\n";
            cout << '<' << CPU::id() << "> " << "CPU MAX FREQ: " << CPU::max_clock() << "\n";
            cout << '<' << CPU::id() << "> " << "CPU MIN FREQ: " << CPU::min_clock() << "\n";
            cout << '<' << CPU::id() << "> " << "CPU FREQ: " << CPU::clock() << "\n";
            cout << '<' << CPU::id() << "> " << "\n========================================================\n";
            _ostream_lock.release();
        }
        return;
    }

    unsigned int slack = (absolute_deadline - current_time) * 10;
    unsigned int relative_deadline = absolute_deadline - start_time;

    unsigned int time_fraction = 10 - slack / relative_deadline;

    /*
    Intel has 8 frequency levels.

    They probably are based on multiples of 12.5, so:
    Level 1 = 12.5% of frequency range
    Level 2 = 25.0% of frequency range
    ...
    and so on.

    For values of time_fraction:
        < 10% --> level 1
        < 20% --> level 2
        < 30% --> level 2
        < 40% --> level 3
        < 50% --> level 5
        < 60% --> level 6
        < 70% --> level 7
        < 80% -> level 7
        >= 80% -> level 8
    */
    int level;
    switch (time_fraction)
    {
    case 0: // < 10%
        level = 1;
        break;
    case 1: // < 20%
        level = 2;
        break;
    case 2: // < 30%
        level = 2;
        break;
    case 3: // < 40%
        level = 3;
        break;
    case 4: // < 50%
        level = 5;
        break;
    case 5: // < 60%
        level = 6;
        break;
    case 6: // < %70
        level = 7;
        break;
    case 7: // < 80%
        level = 7;
        break;
    default: // >= %80
        level = 8;
    }

    Hertz delta = _max_frequency - _min_frequency;
    _step = delta % 8 == 0 ? delta / 8 : delta / 8 + 1;

    Hertz frequency = _min_frequency + level * _step;
    if (frequency > CPU::max_clock())
        frequency = CPU::max_clock();

    CPU::clock(frequency);
    _statistics.average_frequency =  ((unsigned long long)_statistics.average_frequency * (_statistics.num_charges - 1) + frequency)/_statistics.num_charges;
    if (Traits<EDF_Modified>::ENABLE_STATISTICS)
    {
        _ostream_lock.acquire();
        cout << '<' << CPU::id() << "> " << "\n========================================================\n";
        cout << '<' << CPU::id() << "> " << "CURRENT: " << current_time << ", START: " << start_time << ", AB DL: " << absolute_deadline << "\n";
        cout << '<' << CPU::id() << "> " << "TF: " << time_fraction << ", SLACK: " << slack << ", REAL DL: " << relative_deadline << "\n";
        cout << '<' << CPU::id() << "> " << "LEVEL: " << level << ", step: " << _step << ", CLOCK: " << frequency << ", MIN FREQ: " << _min_frequency << "\n";
        cout << '<' << CPU::id() << "> " << "MAX FREQ: " << CPU::max_clock() << "\n";
        cout << '<' << CPU::id() << "> " << "NEW FREQ: " << CPU::clock() << "\n";
        cout << '<' << CPU::id() << "> " << "FREQ PERCENTAGE: " << ((frequency - CPU::min_clock()) * 100.0) / (CPU::max_clock() - CPU::min_clock()) << "%\n";
        cout << '<' << CPU::id() << "> " << "\n========================================================\n";
        _ostream_lock.release();
    }
}

void EDF_Modified::handle(Event event)
{
    RT_Common::handle(event);

    if (periodic() && (event & JOB_RELEASE))
    {
        // Update the priority of the thread at job releases, before _alarm->v(), so it enters the queue in the right order (called from Periodic_Thread::Xxx_Handler)
        int task_type = (BEST_EFFORT & _priority) == BEST_EFFORT ? BEST_EFFORT : CRITICAL;
        _last_deadline = int(elapsed());
        _priority = int(elapsed() + _deadline) | task_type;
        _calculate_min_frequency();
        if (Traits<EDF_Modified>::ENABLE_STATISTICS)
        {
            _ostream_lock.acquire();
            cout << '<' << CPU::id() << "> " << "MIN FREQ: " << _min_frequency << '\n';
            _ostream_lock.release();
        }
    }

    if (!periodic() && (event & FINISH))
    {
        if (Traits<EDF_Modified>::ENABLE_STATISTICS)
        {
            _ostream_lock.acquire();
            cout << '<' << CPU::id() << "> " << "THREAD APERIODIC FINISHED: CPU WILL BE HALTED TO PRESERVE ENERGY" << '\n';
            _ostream_lock.release();
        }
    }

    if (event & CHARGE)
        _handle_charge(event);
}

volatile unsigned long long PEDF_Modified::branch_miss_per_cpu[Traits<Machine>::CPUS];
volatile unsigned long long PEDF_Modified::cache_miss_per_cpu[Traits<Machine>::CPUS];
volatile unsigned long long PEDF_Modified::cpu_usage_per_cpu[Traits<Machine>::CPUS];
volatile unsigned long long PEDF_Modified::base_time[Traits<Machine>::CPUS];
volatile unsigned long long PEDF_Modified::time_spent_in_idle[Traits<Machine>::CPUS];
volatile unsigned long long PEDF_Modified::total_time_of_jobs_per_cpu[Traits<Machine>::CPUS];
volatile unsigned long long PEDF_Modified::total_utilization_per_cpu[Traits<Machine>::CPUS];
volatile unsigned long long PEDF_Modified::utilization_per_cpu[Traits<Machine>::CPUS];
volatile unsigned long long PEDF_Modified::threads_per_cpu[Traits<Machine>::CPUS];

void PEDF_Modified::handle(Event event)
{
    EDF_Modified::handle(event);

    if (periodic() && (event & JOB_FINISH)) {
        //Gets the new reads of PMU
        int branch_miss = (PMU::read(5) > 0) ? (PMU::read(6)*10000ull)/PMU::read(5) : 0;
        int cache_miss = (PMU::read(3) > 0)? (PMU::read(4)*10000ull)/PMU::read(3) : 0;
        
        //Calculates mean of the last 10
        branch_miss_per_cpu[CPU::id()] = (branch_miss_per_cpu[CPU::id()] * 9 + branch_miss)/10;
        cache_miss_per_cpu[CPU::id()] = (cache_miss_per_cpu[CPU::id()] * 9 + cache_miss)/10;

        //Gets the new reads of TSC
        unsigned long long total = TSC::time_stamp() - base_time[CPU::id()];
        //Calculates mean of the last 10
        cpu_usage_per_cpu[CPU::id()] = (total > 0) ? ((cpu_usage_per_cpu[CPU::id()]*9 + (10000ull*(total - time_spent_in_idle[CPU::id()]))))/10 : 0;

        Tick task_type = ((BEST_EFFORT & _priority) == BEST_EFFORT ? BEST_EFFORT : CRITICAL);
        Tick absolute_deadline = _priority - task_type;

        total_time_of_jobs_per_cpu[CPU::id()] += absolute_deadline - _statistics.job_release;
        total_time_of_jobs += absolute_deadline - _statistics.job_release;


        if (absolute_deadline >= _statistics.job_finish) {
            total_utilization_per_cpu[CPU::id()] += _statistics.job_finish - _statistics.job_release;
            total_utilization += _statistics.job_finish - _statistics.job_release;
        } else {
            total_utilization_per_cpu[CPU::id()] += absolute_deadline - _statistics.job_release;
            total_utilization += absolute_deadline - _statistics.job_release;
        }

        if (total_time_of_jobs_per_cpu[CPU::id()] > 0) {
            utilization_per_cpu[CPU::id()] = (10000ull * total_utilization_per_cpu[CPU::id()]) /     
                                                         total_time_of_jobs_per_cpu[CPU::id()];
        } else {
            utilization_per_cpu[CPU::id()] = 0;
        }

    }

    /*
    Removes the total_time and utilization_time of the thread
    to not impact the values to future choose_queue() 
    for remain threads;
    */
    if (periodic() && (event & FINISH)) {
        reset_utilization();
        reset_pmu();
    }

}
volatile unsigned int PEDF_Modified::should_change_queue() {
    if (periodic() && _statistics.number_dispatches % 5 == 0 && _statistics.number_dispatches > 0) {
        volatile unsigned int q = choose_queue();
        if (q != _queue) {
            /*
            Removes the total_time and slack of the thread
            to not impact the values to future choose_queue() 
            for remain threads of old_cpu;
            */
            reset_utilization();                                                                                 
            total_time_of_jobs = 0;
            total_utilization = 0;
            reset_pmu();
            return q;
        }
    }
    return 1e9;
}

void PEDF_Modified::reset_utilization() {
    if (total_time_of_jobs_per_cpu[CPU::id()] > (long long unsigned int) total_time_of_jobs) {
        total_time_of_jobs_per_cpu[CPU::id()] -= total_time_of_jobs;
    } else {
        total_time_of_jobs_per_cpu[CPU::id()] = 0;
    }
    if (total_utilization_per_cpu[CPU::id()] > (long long unsigned int) total_utilization) {
        total_utilization_per_cpu[CPU::id()] -= total_utilization;
    } else {
        total_utilization_per_cpu[CPU::id()] = 0;
    }
    if (total_time_of_jobs_per_cpu[CPU::id()] > 0) {
        utilization_per_cpu[CPU::id()] = (10000ull * total_utilization_per_cpu[CPU::id()])/total_time_of_jobs_per_cpu[CPU::id()];
    } else {
        utilization_per_cpu[CPU::id()] = 0;
    }          
}

void PEDF_Modified::reset_pmu() {
    PMU::reset(6);
    PMU::reset(5);
    PMU::reset(4);
    PMU::reset(3);
    PMU::config(6, 15); // BRANCH_MISSES_RETIRED
    PMU::config(5, 11); // BRANCH_INSTRUCTIONS_RETIRED
    PMU::config(4, 21); // L1 MISSES
    PMU::config(3, 20); // L1 HITS
    PMU::start(6);
    PMU::start(5);
    PMU::start(4);
    PMU::start(3);

    base_time[CPU::id()] = TSC::time_stamp();
    time_spent_in_idle[CPU::id()] = 0;
}

// The following Scheduling Criteria depend on Alarm, which is not available at scheduler.h
template <typename... Tn>
FCFS::FCFS(int p, Tn &...an) : Priority((p == IDLE) ? IDLE : Alarm::elapsed()) {}

// Since the definition above is only known to this unit, forcing its instantiation here so it gets emitted in scheduler.o for subsequent linking with other units is necessary.
template FCFS::FCFS<>(int p);

__END_SYS
