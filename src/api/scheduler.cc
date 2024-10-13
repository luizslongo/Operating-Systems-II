// EPOS CPU Scheduler Component Implementation

#include "scheduler.h"
#include "system/config.h"
#include "system/traits.h"
#include <utility/math.h>
#include <process.h>
#include <time.h>

__BEGIN_SYS

inline RT_Common::Tick RT_Common::elapsed() { return Alarm::elapsed(); }

RT_Common::Tick RT_Common::ticks(Microsecond time) {
    return Timer_Common::ticks(time, Alarm::timer()->frequency());
}

Microsecond RT_Common::time(Tick ticks) {
    return Timer_Common::time(ticks, Alarm::timer()->frequency());
}

void RT_Common::handle(Event event) {
    db<Thread>(TRC) << "RT::handle(this=" << this << ",e=";
    if(event & CREATE) {
        db<Thread>(TRC) << "CREATE";

        _statistics.thread_creation = elapsed();
        _statistics.job_released = false;
    }
    if(event & FINISH) {
        db<Thread>(TRC) << "FINISH";

        _statistics.thread_destruction = elapsed();
    }
    if(event & ENTER) {
        db<Thread>(TRC) << "ENTER";

        _statistics.thread_last_dispatch = elapsed();
    }
    if(event & LEAVE) {
        Tick cpu_time = elapsed() - _statistics.thread_last_dispatch;

        db<Thread>(TRC) << "LEAVE";

        _statistics.thread_last_preemption = elapsed();
        _statistics.thread_execution_time += cpu_time;
        _statistics.job_utilization += cpu_time;
//        if(_statistics.job_released) {
//        }
    }
    if(periodic() && (event & JOB_RELEASE)) {
        db<Thread>(TRC) << "RELEASE";

        if (_statistics.jobs_released < 5)
            _statistics.average_job_execution_time += _statistics.job_utilization/5;
        else {
            _statistics.average_job_execution_time = (_statistics.average_job_execution_time*4 + _statistics.job_utilization)/5;
        }
        
        OStream cout;
        cout << "JOB AVERAGE EXECUTION TIME (last 5 executions): " << _statistics.average_job_execution_time << '\n';
        cout << "JOB UTILIZATION: " << _statistics.job_utilization << '\n';
        _statistics.job_released = true;
        _statistics.job_release = elapsed();
        _statistics.job_start = 0;
        _statistics.job_utilization = 0;
        _statistics.jobs_released++;
    }
    if(periodic() && (event & JOB_FINISH)) {
        db<Thread>(TRC) << "WAIT";

        _statistics.job_released = false;
        _statistics.job_finish = elapsed();
        _statistics.jobs_finished++;
//        _statistics.job_utilization += elapsed() - _statistics.thread_last_dispatch;
    }
    if(event & COLLECT) {
        db<Thread>(TRC) << "|COLLECT";
    }
    if(periodic() && (event & CHARGE)) {
        db<Thread>(TRC) << "|CHARGE";
    }
    if(periodic() && (event & AWARD)) {
        db<Thread>(TRC) << "|AWARD";
    }
    if(periodic() && (event & UPDATE)) {
        db<Thread>(TRC) << "|UPDATE";
    }
    db<Thread>(TRC) << ") => {i=" << _priority << ",p=" << _period << ",d=" << _deadline << ",c=" << _capacity << "}" << endl;
}


template <typename ... Tn>
FCFS::FCFS(int p, Tn & ... an): Priority((p == IDLE) ? IDLE : RT_Common::elapsed()) {}


EDF::EDF(Microsecond p, Microsecond d, Microsecond c, int& a): RT_Common(int(elapsed() + ticks(d)), p, d, c) {}

void EDF::handle(Event event) {
    RT_Common::handle(event);

    // Update the priority of the thread at job releases, before _alarm->v(), so it enters the queue in the right order (called from Periodic_Thread::Xxx_Handler)
    if(periodic() && (event & JOB_RELEASE))
        _priority = elapsed() + _deadline;
}

LLF::LLF(Microsecond p, Microsecond d, Microsecond c, int &a): RT_Common(int(elapsed() + ticks((d ? d : p) - c)), p, d, c) {}

void LLF::handle(Event event) {
    if(periodic() && ((event & UPDATE) | (event & JOB_RELEASE) | (event & JOB_FINISH))) {
        _priority = elapsed() + _deadline - _capacity + _statistics.job_utilization;
    }
    RT_Common::handle(event);

    // Update the priority of the thread at job releases, before _alarm->v(), so it enters the queue in the right order (called from Periodic_Thread::Xxx_Handler)
//    if((_priority >= PERIODIC) && (_priority < APERIODIC) && ((event & JOB_FINISH) || (event & UPDATE_ALL)))
}


EDF_Modified::EDF_Modified(Microsecond p, Microsecond d, Microsecond c, int task_type): RT_Common(int(elapsed() + ticks(d)) | task_type, p, d, c), _min_frequency(CPU::max_clock()), _max_frequency(CPU::max_clock()), _last_deadline(elapsed()), _step(_max_frequency ) {
    memset(&_statistics, 0, sizeof(_statistics));
    // _statistics.thread_last_dispatch = _statistics.thread_last_preemption = _statistics.jobs_released = _statistics.job_utilization = _statistics.average_job_execution_time = 0;
}

void EDF_Modified::_calculate_min_frequency() {
    if (_statistics.jobs_released < 5)
        return;

    unsigned int average_execution_fraction = (_statistics.average_job_execution_time * 10)/_deadline;
    
    OStream cout;
    cout << "AVERAGE EF: " << average_execution_fraction << ", _DEADLINE: " << _deadline << '\n';

    if (_statistics.jobs_released == 5) {
        Hertz step = (CPU::max_clock() - CPU::min_clock())/10;
        _min_frequency = CPU::min_clock() + step*average_execution_fraction;
        return;
    }
    // fazemos dessa forma pra ter uma mudança mais previsível no min_cpu_frequency. 
    // Pensei em fazer matematicamente, mas n achei nenhuma forma boa.
    switch (average_execution_fraction) {
        case 0:
        case 1:
            _min_frequency -= _min_frequency/4; //diminui 25%
            break; //entre 0% e 20%
        case 2:
        case 3:
            _min_frequency -= _min_frequency/8; //diminui 12.5%
            break; //entre 20% e 40%
        case 4:
        case 5:
        case 6:
            break; //entre 40% e 70% 
        case 7:
        case 8:
            _min_frequency += _min_frequency/8; //aumenta 12.5%
            break; //entre 70% e 90%
        default:
            _min_frequency += _min_frequency/4; //aumenta 25%
            //acima de 90%
        
    }

    //Resolvendo problemas de contorno
    if (_min_frequency < CPU::min_clock())
        _min_frequency = CPU::min_clock();
    else if (_min_frequency > CPU::max_clock())
        _min_frequency = CPU::max_clock();
}

void EDF_Modified::_handle_charge(Event event) {
    EPOS::OStream cout;
    if (!periodic()) {
        if (CPU::clock() != _min_frequency) {
            cout << "Aperiodic Task!!! Putting CPU on MIN FREQUENCY\n";
            CPU::clock(_min_frequency);
        }
        return;
    }
    
    unsigned int absolute_deadline = _priority;
    unsigned int start_time = _last_deadline;
    unsigned int current_time = elapsed();
    
    if (current_time > absolute_deadline) {
        cout << "\n========================================================\n";
        cout << "Deadline Missed!!! Putting CPU on MAX FREQUENCY\n";
        cout << "CURRENT: " << current_time << ", START: " << start_time << ", AB DL: " << absolute_deadline << "\n";
        cout << "\n========================================================\n";
        CPU::clock(_max_frequency);
        return;
    }
    
    unsigned int slack = (absolute_deadline - current_time)*10;
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
    switch (time_fraction) {
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
    _step = delta % 8 == 0 ? delta/8 : delta/8 + 1;

    Hertz frequency = _min_frequency + level*_step;
    if (frequency > CPU::max_clock())
        frequency = CPU::max_clock();

    CPU::clock(frequency);
    cout << "\n========================================================\n";
    cout << "CURRENT: " << current_time << ", START: " << start_time << ", AB DL: " << absolute_deadline << "\n";
    cout << "TF: " << time_fraction << ", SLACK: " << slack << ", REAL DL: " << relative_deadline << "\n";
    cout << "LEVEL: " << level << ", step: " << _step << ", CLOCK: " << frequency << ", MIN FREQ: " << _min_frequency << "\n";
    cout << "FREQ PERCENTAGE: " << ((frequency - CPU::min_clock())*100.0)/(CPU::max_clock() - CPU::min_clock()) << "%\n";
    cout << "\n========================================================\n";

}

void EDF_Modified::handle(Event event) {

    if(periodic() && (event & JOB_RELEASE)) {
        // Update the priority of the thread at job releases, before _alarm->v(), so it enters the queue in the right order (called from Periodic_Thread::Xxx_Handler)
        int task_type = (BEST_EFFORT & _priority) == BEST_EFFORT ? BEST_EFFORT : CRITICAL;
        _last_deadline = int(elapsed());
        _priority = int(elapsed() + _deadline) | task_type;
        _calculate_min_frequency();
        OStream cout;
        cout << "MIN FREQ: " << _min_frequency << '\n';
    }

    if (((event & CHARGE) && !(event & LEAVE)) || (event & ENTER))
        _handle_charge(event);

    RT_Common::handle(event);
}

// Since the definition of FCFS above is only known to this unit, forcing its instantiation here so it gets emitted in scheduler.o for subsequent linking with other units is necessary.
template FCFS::FCFS<>(int p);

__END_SYS
