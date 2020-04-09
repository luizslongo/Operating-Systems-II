// EPOS Thread Implementation

#include <machine.h>
#include <system.h>
#include <process.h>
#include <clerk.h>

// This_Thread class attributes
__BEGIN_UTIL
bool This_Thread::_not_booting;
__END_UTIL

__BEGIN_SYS

// Class attributes
volatile unsigned int Thread::_thread_count;
Scheduler_Timer * Thread::_timer;
Scheduler<Thread> Thread::_scheduler;
Spin Thread::_lock;


// Statistics
TSC::Time_Stamp Thread::_Statistics::hyperperiod[Traits<Build>::CPUS];
TSC::Time_Stamp Thread::_Statistics::wcet_cpu[Traits<Build>::CPUS];
TSC::Time_Stamp Thread::_Statistics::last_hyperperiod[Traits<Build>::CPUS];
unsigned int Thread::_Statistics::hyperperiod_count[Traits<Build>::CPUS];
TSC::Time_Stamp Thread::_Statistics::hyperperiod_idle_time[Traits<Build>::CPUS];
TSC::Time_Stamp Thread::_Statistics::idle_time[Traits<Build>::CPUS];
TSC::Time_Stamp Thread::_Statistics::last_idle[Traits<Build>::CPUS];

// ANN
bool Thread::_Statistics::decrease_frequency[Traits<Build>::CPUS] = {true, true, true, true};
unsigned int Thread::_Statistics::count_ann[Traits<Build>::CPUS];
unsigned int Thread::_Statistics::size_ann[Traits<Build>::CPUS];
float Thread::_Statistics::ann_inputs[Traits<Build>::CPUS][50][COUNTOF(Traits<Monitor>::PMU_EVENTS)+COUNTOF(Traits<Monitor>::SYSTEM_EVENTS)];
float Thread::_Statistics::ann_outputs[Traits<Build>::CPUS][50][3];
unsigned int Thread::_Statistics::missed_deadlines[Traits<Build>::CPUS];

unsigned long long Thread::_Statistics::cpu_pmu_accumulated[Traits<Build>::CPUS][COUNTOF(Traits<Monitor>::PMU_EVENTS)];
unsigned long long Thread::_Statistics::cpu_pmu_last[Traits<Build>::CPUS][COUNTOF(Traits<Monitor>::PMU_EVENTS)];
bool Thread::_Statistics::cooldown[Traits<Build>::CPUS];


// Methods
void Thread::constructor_prologue(const Color & color, unsigned int stack_size)
{
    lock();

    _thread_count++;
    _scheduler.insert(this);

    if(Traits<MMU>::colorful && color != WHITE)
        _stack = new (color) char[stack_size];
    else
        _stack = new (SYSTEM) char[stack_size];
}


void Thread::constructor_epilogue(const Log_Addr & entry, unsigned int stack_size)
{
    if (_link.rank() == IDLE || _link.rank() == MAIN)
        db<Thread>(TRC) << "Thread(task=" << _task
                    << ",entry=" << entry
                    << ",state=" << _state
                    << ",priority=" << _link.rank()
                    << ",stack={b=" << reinterpret_cast<void *>(_stack)
                    << ",s=" << stack_size
                    << "},context={b=" << _context
                    << "," << *_context << "}) => " << this << "@" << _link.rank().queue() << endl;

    for(unsigned int i = 0; i < COUNTOF(Traits<Monitor>::PMU_EVENTS); i++)
        _statistics.thread_pmu_accumulated[i] = 0;
    if(multitask)
        _task->insert(this);

    if((_state != READY) && (_state != RUNNING))
        _scheduler.suspend(this);

    if(preemptive && (_state == READY) && (_link.rank() != IDLE))
        reschedule(_link.rank().queue());
    else
        unlock();
}


Thread::~Thread()
{
    lock();

    db<Thread>(TRC) << "~Thread(this=" << this
                    << ",state=" << _state
                    << ",priority=" << _link.rank()
                    << ",stack={b=" << reinterpret_cast<void *>(_stack)
                    << ",context={b=" << _context
                    << "," << *_context << "})" << endl;

    // The running thread cannot delete itself!
    assert(_state != RUNNING);

    switch(_state) {
    case RUNNING:  // For switch completion only: the running thread would have deleted itself! Stack wouldn't have been released!
        exit(-1);
        break;
    case READY:
        _scheduler.remove(this);
        _thread_count--;
        break;
    case SUSPENDED:
        _scheduler.resume(this);
        _scheduler.remove(this);
        _thread_count--;
        break;
    case WAITING:
        _waiting->remove(this);
        _scheduler.resume(this);
        _scheduler.remove(this);
        _thread_count--;
        break;
    case FINISHING: // Already called exit()
        break;
    }

    if(multitask) {
        _task->remove(this);
        delete _user_stack;
    }

    if(_joining)
        _joining->resume();

    unlock();

    delete _stack;
}


void Thread::priority(const Criterion & c)
{

    /*
    * Problem description
    * when changing the priority of a RUNNING Thread, the _scheduler.remove makes the thread be removed from the queue,
    * but when calling reschedule(), the thread set as the running thread is the running(), which is the scheduler.chosen()
    * this makes the dispatch to switch context with the wrong thread
    * the scenarios to pay attention are:
    * 1: Core 1 calls priority for a running thread "t0" on Core 2:
    *   1.1: t0's new priority does not change the running Core
    *   1.2: t0's new priority changes the running Core
    * 2: Core 1 calls priority for its own running thread
    */
    lock();

    db<Thread>(TRC) << "Thread::priority(this=" << this << ",prio=" << c << ")" << endl;

//    unsigned int old_cpu = _link.rank().queue();

    if(_state != RUNNING) { // reorder the scheduling queue
        _scheduler.remove(this);
        _link = Queue::Element(this, c);
        _scheduler.insert(this);
    } else
        _link = Queue::Element(this, c);

    unsigned int new_cpu = _link.rank().queue();

    if(preemptive) {
//        if(old_cpu != CPU::id()) {
//            reschedule(old_cpu);
            if(smp) {
                lock();
                reschedule(new_cpu);
            }
//        } else if(new_cpu != CPU::id()) {
//            reschedule(new_cpu);
//            if(smp) {
//                lock();
//                reschedule(old_cpu);
//            }
//        } else
//            reschedule();
    }

    unlock();
}


int Thread::join()
{
    lock();

    db<Thread>(TRC) << "Thread::join(this=" << this << ",state=" << _state << ")" << endl;

    // Precondition: no Thread::self()->join()
    assert(running() != this);

    // Precondition: a single joiner
    assert(!_joining);

    if(_state != FINISHING) {
        _joining = running();
        _joining->suspend(true);
    } else
        unlock();

    return *reinterpret_cast<int *>(_stack);
}


void Thread::pass()
{
    lock();

    db<Thread>(TRC) << "Thread::pass(this=" << this << ")" << endl;

    Thread * prev = running();
    Thread * next = _scheduler.choose(this);

    if(next)
        dispatch(prev, next, false);
    else {
        db<Thread>(WRN) << "Thread::pass => thread (" << this << ") not ready!" << endl;
        unlock();
    }
}


void Thread::suspend(bool locked)
{
    if(!locked)
        lock();

    db<Thread>(TRC) << "Thread::suspend(this=" << this << ")" << endl;

    Thread * prev = running();

    _state = SUSPENDED;
    _scheduler.suspend(this);

    Thread * next = running();

    dispatch(prev, next);
}


void Thread::resume()
{
    lock();

    db<Thread>(TRC) << "Thread::resume(this=" << this << ")" << endl;

    if(_state == SUSPENDED) {
        _state = READY;
        _scheduler.resume(this);

        if(preemptive)
            reschedule(_link.rank().queue());
        else
            unlock();
    } else {
        db<Thread>(WRN) << "Resume called for unsuspended object!" << endl;

        unlock();
    }
}


// Class methods
void Thread::yield()
{
    lock();

    db<Thread>(TRC) << "Thread::yield(running=" << running() << ")" << endl;

    Thread * prev = running();
    Thread * next = _scheduler.choose_another();

    dispatch(prev, next);
}


void Thread::exit(int status)
{
    lock();

    db<Thread>(TRC) << "Thread::exit(status=" << status << ") [running=" << running() << "]" << endl;

    Thread * prev = running();
    _scheduler.remove(prev);
    prev->_state = FINISHING;
    *reinterpret_cast<int *>(prev->_stack) = status;

    _thread_count--;

    if(prev->_joining) {
        prev->_joining->_state = READY;
        _scheduler.resume(prev->_joining);
        prev->_joining = 0;
    }

    dispatch(prev, _scheduler.choose()); // at least idle will always be there
}


void Thread::sleep(Queue * q)
{
    db<Thread>(TRC) << "Thread::sleep(running=" << running() << ",q=" << q << ")" << endl;

    // lock() must be called before entering this method
    assert(locked());

    Thread * prev = running();
    _scheduler.suspend(prev);
    prev->_state = WAITING;
    prev->_waiting = q;
    q->insert(&prev->_link);

    dispatch(prev, _scheduler.chosen());
}


void Thread::wakeup(Queue * q)
{
    db<Thread>(TRC) << "Thread::wakeup(running=" << running() << ",q=" << q << ")" << endl;

    // lock() must be called before entering this method
    assert(locked());

    if(!q->empty()) {
        Thread * t = q->remove()->object();
        t->_state = READY;
        t->_waiting = 0;
        _scheduler.resume(t);

        if(preemptive)
            reschedule(t->_link.rank().queue());
        else
            unlock();
    } else
        unlock();
}


void Thread::wakeup_all(Queue * q)
{
    db<Thread>(TRC) << "Thread::wakeup_all(running=" << running() << ",q=" << q << ")" << endl;

    // lock() must be called before entering this method
    assert(locked());

    if(!q->empty()) {
        assert(Criterion::QUEUES <= sizeof(unsigned int) * 8);
        unsigned int cpus = 0;
        while(!q->empty()) {
            Thread * t = q->remove()->object();
            t->_state = READY;
            t->_waiting = 0;
            _scheduler.resume(t);
            cpus |= 1 << t->_link.rank().queue();
        }

        if(preemptive) {
            for(unsigned int i = 0; i < Criterion::QUEUES; i++)
                if(cpus & (1 << i))
                    reschedule(i);
        } else
            unlock();
    } else
        unlock();
}


void Thread::reschedule()
{
    if(!Criterion::timed || Traits<Thread>::hysterically_debugged)
        db<Thread>(TRC) << "Thread::reschedule()" << endl;

    // lock() must be called before entering this method
    assert(locked());

    Thread * prev = running();
    Thread * next = _scheduler.choose();

    dispatch(prev, next);
}


void Thread::reschedule(unsigned int cpu)
{
    if(!smp || (cpu == CPU::id()))
        reschedule();
    else {
        db<Thread>(TRC) << "Thread::reschedule(cpu=" << cpu << ")" << endl;
        IC::ipi(cpu, IC::INT_RESCHEDULER);
        unlock();
    }
}


void Thread::rescheduler(IC::Interrupt_Id i)
{
    lock();

    reschedule();
}


void Thread::time_slicer(IC::Interrupt_Id i)
{
    lock();

    reschedule();
}


void Thread::dispatch(Thread * prev, Thread * next, bool charge)
{
    if(charge) {
        if(Criterion::timed)
            _timer->reset();
    }

    if(monitored && Monitor::is_enable()) {
        unsigned int cpu = CPU::id();
        Simple_List<Monitor> *monitor = &(Monitor::_monitors[cpu]);
        TSC::Time_Stamp ts = TSC::time_stamp();

        unsigned long long captures[COUNTOF(Traits<Monitor>::PMU_EVENTS)];
        unsigned int i = 0;

        // PMU account aux
        for(Simple_List<Monitor>::Iterator it = monitor->begin(); it != monitor->end() && i < COUNTOF(Traits<Monitor>::PMU_EVENTS); it++){
            it->object()->stop(); // does not account this in trace
            captures[i] = it->object()->read();
            i++;
        }

        // Next PMU account checkpoint
        if((next->priority() > Criterion::PERIODIC) && (next->priority() < Criterion::APERIODIC)) {
            for(i = 0; i < COUNTOF(Traits<Monitor>::PMU_EVENTS); i++) {
                next->_statistics.thread_pmu_last[i] = captures[i];
            }
        }

        // Prev PMU account
        if((prev->priority() > Criterion::PERIODIC) && (prev->priority() < Criterion::APERIODIC)) {
            for(i = 0; i < COUNTOF(Traits<Monitor>::PMU_EVENTS); i++) {
                if (captures[i] < prev->_statistics.thread_pmu_last[i]) { // counter reset
                    // captures + 2**64 - 1 - last 
                    prev->_statistics.thread_pmu_accumulated[i] += captures[i] + ((unsigned long long) 0xffffffff) - prev->_statistics.thread_pmu_last[i];
                } else {
                    prev->_statistics.thread_pmu_accumulated[i] += captures[i] - prev->_statistics.thread_pmu_last[i];
                }
            }
        }

        if(INARRAY(Traits<Monitor>::SYSTEM_EVENTS, Traits<Monitor>::CPU_EXECUTION_TIME) || INARRAY(Traits<Monitor>::SYSTEM_EVENTS, Traits<Monitor>::CPU_WCET)) {
            // Account idle time
            if((prev->priority() == IDLE) && (prev->_statistics.last_idle[cpu] != 0)) {
                Thread::_Statistics::idle_time[cpu] += ts - Thread::_Statistics::last_idle[cpu];
            }

            // Idle time Checkpoint 
            if(next->priority() == IDLE) {
                Thread::_Statistics::last_idle[cpu] = ts;
            }
        }

        if(INARRAY(Traits<Monitor>::SYSTEM_EVENTS, Traits<Monitor>::THREAD_EXECUTION_TIME)) { // TODO reset hyperperiod
            if((prev->priority() > Criterion::PERIODIC) && (prev->priority() < Criterion::APERIODIC)) { // a real-time thread
                // Account Thread execution time
                if (prev->_statistics.last_execution != 0) {
                    //if (prev->_statistics.hyperperiod_count_thread < prev->_statistics.hyperperiod_count[cpu]) {
                    //    prev->_statistics.hyperperiod_count_thread = prev->_statistics.hyperperiod_count[cpu];
                    //    prev->_statistics.execution_time = ts - prev->_statistics.last_execution;
                    //    prev->_statistics.average_execution_time = 0;
                    //    prev->_statistics.jobs = 1; 
                    //} else {
                        prev->_statistics.execution_time += ts - prev->_statistics.last_execution;
                    //}
                }
            }
            // Account Thread execution time checkpoint
            if((next->priority() > Criterion::PERIODIC) && (next->priority() < Criterion::APERIODIC)) {
                //if (next->_statistics.hyperperiod_count_thread < next->_statistics.hyperperiod_count[cpu]) {
                //    next->_statistics.hyperperiod_count_thread = next->_statistics.hyperperiod_count[cpu];
                //    next->_statistics.execution_time = 0;
                //    next->_statistics.average_execution_time = 0;
                //    next->_statistics.jobs = 0;
                //}
                next->_statistics.last_execution = ts;
                //next->_statistics.hyperperiod_count_thread = next->_statistics.hyperperiod_count[cpu];
            }
        }
        Monitor::run();

        // CPU hyperperiod account
        if (ts >= Thread::_Statistics::hyperperiod[cpu] + Thread::_Statistics::last_hyperperiod[cpu] && Thread::_Statistics::hyperperiod[cpu] != 0) {
            // hyper-period count
            Thread::_Statistics::hyperperiod_count[cpu]++;
            Thread::_Statistics::last_hyperperiod[cpu] = ts;
            // hyper-period idle time
            Thread::_Statistics::hyperperiod_idle_time[cpu] = Thread::_Statistics::idle_time[cpu];
            // reset current idle time (for next hyperperiod)

            for(i = 0; i < COUNTOF(Traits<Monitor>::PMU_EVENTS); i++)
            {
                if (captures[i] < prev->_statistics.thread_pmu_last[i]) { // counter reset // TODO reset pmu count on hyperperiod (change += to =)
                    // captures + 2**64 - 1 - last 
                    Thread::_Statistics::cpu_pmu_accumulated[cpu][i] += captures[i] + ((unsigned long long) 0xffffffff) - Thread::_Statistics::cpu_pmu_last[cpu][i];
                } else {
                    Thread::_Statistics::cpu_pmu_accumulated[cpu][i] += captures[i] - Thread::_Statistics::cpu_pmu_last[cpu][i];
                }
                Thread::_Statistics::cpu_pmu_last[cpu][i] = captures[i];
            }
            //Criterion::charge(); // run ANN
            if(cpu) {
                Criterion::award(true); // in: hyper-period? | out: true = decrease; false = increase or maintain;
                Thread::_Statistics::decrease_frequency[cpu] = true; // reset decrease frequency for next hyperperiod
            }
            Thread::_Statistics::missed_deadlines[cpu] = 0;
            Thread::_Statistics::idle_time[cpu] = 0;
        } else if(cpu && (prev->priority() > Criterion::PERIODIC) && (prev->priority() < Criterion::APERIODIC)) {
            // fix timing
            if(Criterion::charge()) // if run ANN
                Thread::_Statistics::decrease_frequency[cpu] &= Criterion::award(false); // in: hyper-period? | out: true = decrease; false = increase or maintain;
        }

        // PMU start
        for(Simple_List<Monitor>::Iterator it = monitor->begin(); it != monitor->end(); it++){
            it->object()->start();
        }

    }

    if(prev != next) {
        if(prev->_state == RUNNING)
            prev->_state = READY;
        next->_state = RUNNING;

        db<Thread>(TRC) << "Thread::dispatch(prev=" << prev << ",next=" << next << ")" << endl;
        db<Thread>(INF) << "prev={" << prev << ",ctx=" << *prev->_context << "}" << endl;
        db<Thread>(INF) << "next={" << next << ",ctx=" << *next->_context << "}" << endl;

        if(smp)
            _lock.release();

        if(multitask && (next->_task != prev->_task))
            next->_task->activate();

        // The non-volatile pointer to volatile pointer to a non-volatile context is correct
        // and necessary because of context switches, but here, we are locked() and
        // passing the volatile to switch_constext forces it to push prev onto the stack,
        // disrupting the context (it doesn't make a difference for Intel, which already saves
        // parameters on the stack anyway).
        CPU::switch_context(const_cast<Context **>(&prev->_context), next->_context);
    } else
        if(smp)
            _lock.release();

    CPU::int_enable();
}

void Thread::reset_statistics() {
    Thread * t = running();
    for (unsigned int i = 0; i < Traits<Build>::CPUS; ++i)
    {
        t->_statistics.hyperperiod[i] = 0;
        t->_statistics.last_hyperperiod[i] = 0;
        t->_statistics.hyperperiod_count[i] = 0;
        t->_statistics.cooldown[i] = 0;
        t->_statistics.hyperperiod_idle_time[i] = 0;
        t->_statistics.idle_time[i] = 0;
        t->_statistics.last_idle[i] = 0;
        for (unsigned int j = 0; j < COUNTOF(Traits<Monitor>::PMU_EVENTS); ++j)
        {
            t->_statistics.cpu_pmu_accumulated[i][j] = 0;
            t->_statistics.cpu_pmu_last[i][j] = 0;
        }
    }
}


int Thread::idle()
{
    db<Thread>(TRC) << "Thread::idle(cpu=" << CPU::id() << ",this=" << running() << ")" << endl;

    while(_thread_count > CPU::cores()) { // someone else besides idles
        if(Traits<Thread>::trace_idle)
            db<Thread>(TRC) << "Thread::idle(cpu=" << CPU::id() << ",this=" << running() << ")" << endl;
        //db<Thread>(WRN) << "." << endl;
        CPU::int_enable();
        CPU::halt();

        if(_scheduler.schedulables() > 0) // A thread might have been woken up by another CPU
            yield();
    }

    CPU::int_disable();
    if(CPU::id() == 0) {
        if(monitored)
            Monitor::process_batch();

        db<Thread>(WRN) << "The last thread has exited!" << endl;
        if(reboot) {
            db<Thread>(WRN) << "Rebooting the machine ..." << endl;
            //Machine::reboot();
            CPU::halt();
        } else {
            db<Thread>(WRN) << "Halting the machine ..." << endl;
            CPU::halt();
        }
    }

    // Some machines need a little time to reboot
    for(;;);
    
    return 0;
}

__END_SYS

// Id forwarder to the spin lock
__BEGIN_UTIL
unsigned int This_Thread::id()
{
    return _not_booting ? reinterpret_cast<volatile unsigned int>(Thread::self()) : CPU::id() + 1;
}
__END_UTIL
