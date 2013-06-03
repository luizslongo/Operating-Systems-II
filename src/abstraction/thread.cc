// EPOS Thread Abstraction Implementation

#include <machine.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

// Class attributes
volatile unsigned int Thread::_thread_count;
Scheduler_Timer * Thread::_timer;
Scheduler<Thread> Thread::_scheduler;
Spin Thread::_lock;


// This_Thread class attributes
bool This_Thread::_not_booting;


// Methods
void Thread::common_constructor(Log_Addr entry, unsigned int stack_size) 
{
    db<Thread>(TRC) << "Thread(entry=" << entry
                    << ",state=" << _state
                    << ",priority=" << _link.rank()
                    << ",stack={b=" << reinterpret_cast<void*>(_stack)
                    << ",s=" << stack_size
                    << "},context={b=" << _context
                    << "," << *_context << "}) => " << this << "\n";

    _thread_count++;

    _scheduler.insert(this);
    if((_state != READY) && (_state != RUNNING))
        _scheduler.suspend(this);

    if(preemptive && (_state == READY) && (_link.rank() != IDLE))
        reschedule();
    else
        unlock();
}


Thread::~Thread()
{
    lock();

    db<Thread>(TRC) << "~Thread(this=" << this 
                    << ",state=" << _state
                    << ",priority=" << _link.rank()
                    << ",stack={b=" << reinterpret_cast<void*>(_stack)
                    << ",context={b=" << _context
                    << "," << *_context << "})\n";

    switch(_state) {
    case BEGINNING:
        _scheduler.resume(this);
        _thread_count--;
        break;
    case RUNNING:  // Self deleted itself!
        exit(-1);
        break;
    case READY:
        _thread_count--;
        break;
    case SUSPENDED:
        _scheduler.resume(this);
        _thread_count--;
        break;
    case WAITING:
        _waiting->remove(this);
        _scheduler.resume(this);
        _thread_count--;
        break;
    case FINISHING: // Already called exit()
        break;
    }

    _scheduler.remove(this);

    unlock();

    delete _stack;
}


void Thread::priority(const Priority & p)
{
    lock();

    db<Thread>(TRC) << "Thread::priority(this=" << this
                    << ",prio=" << p << ")\n";

    _scheduler.remove(this);
    _link.rank(int(p));
    _scheduler.insert(this);

    if(preemptive)
        reschedule();
}


int Thread::join()
{
    lock();

    db<Thread>(TRC) << "Thread::join(this=" << this
                    << ",state=" << _state << ")\n";

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

    db<Thread>(TRC) << "Thread::pass(this=" << this << ")\n";

    Thread * prev = running();
    Thread * next = _scheduler.choose(this);

    if(next)
        dispatch(prev, next, false);
    else {
        db<Thread>(WRN) << "Thread::pass => thread (" << this << ") not ready\n";
        unlock();
    }
}


void Thread::suspend(bool locked)
{
    if(!locked)
        lock();

    db<Thread>(TRC) << "Thread::suspend(this=" << this << ")\n";

    Thread * prev = running();

    _scheduler.suspend(this);
    _state = SUSPENDED;

    Thread * next = running();

    dispatch(prev, next);
}


void Thread::resume()
{
    lock();

    db<Thread>(TRC) << "Thread::resume(this=" << this << ")\n";

    if(_state == SUSPENDED) {
        _state = READY;
        _scheduler.resume(this);
    } else
        db<Thread>(WRN) << "Resume called for unsuspended object!\n";

    if(preemptive)
        reschedule();
}


// Class methods
void Thread::yield()
{
    lock();

    db<Thread>(TRC) << "Thread::yield(running=" << running() << ")\n";

    Thread * prev = running();
    Thread * next = _scheduler.choose_another();

    dispatch(prev, next);
}


void Thread::exit(int status)
{
    lock();

    db<Thread>(TRC) << "Thread::exit(running=" << running()
                    <<",status=" << status << ")\n";

    Thread * prev = running();
    _scheduler.remove(prev);
    *reinterpret_cast<int *>(prev->_stack) = status;
    prev->_state = FINISHING;

    _thread_count--;

    if(prev->_joining) {
        prev->_joining->_state = READY;
        _scheduler.resume(prev->_joining);
        prev->_joining = 0;
    }

    dispatch(prev, _scheduler.choose());
}


void Thread::sleep(Queue * q)
{
    db<Thread>(TRC) << "Thread::sleep(running=" << running()
                    << ",q=" << q << ")\n";

    Thread * prev = running();
    _scheduler.suspend(prev);
    prev->_state = WAITING;
    q->insert(&prev->_link);
    prev->_waiting = q;

    dispatch(prev, _scheduler.chosen());
}


void Thread::wakeup(Queue * q)
{
    db<Thread>(TRC) << "Thread::wakeup(running=" << running()
                    << ",q=" << q << ")\n";

    if(!q->empty()) {
        Thread * t = q->remove()->object();
        t->_state = READY;
        t->_waiting = 0;
        _scheduler.resume(t);
    }

    if(preemptive)
        reschedule();
    else
        unlock();
}


void Thread::wakeup_all(Queue * q)
{
    db<Thread>(TRC) << "Thread::wakeup_all(running=" << running()
                    << ",q=" << q << ")\n";

    while(!q->empty()) {
        Thread * t = q->remove()->object();
        t->_state = READY;
        t->_waiting = 0;
        _scheduler.resume(t);
    }

    if(preemptive)
        reschedule();
    else
        unlock();
}


void Thread::reschedule()
{
    db<Thread>(TRC) << "Thread::reschedule()\n";

//    for(unsigned int n = 1; n < Machine::n_cpus(); n++)
//        IC::ipi_send(n, IC::INT_TIMER);

    Thread * prev = running();
    Thread * next = _scheduler.choose();

    dispatch(prev, next);
}


void Thread::time_slicer()
{
    lock();

    reschedule();
}


void Thread::implicit_exit() 
{
    exit(CPU::fr()); 
}


int Thread::idle()
{
    while(true) {
        if(Traits<Thread>::trace_idle)
            db<Thread>(TRC) << "Thread::idle(CPU=" << Machine::cpu_id() << ",this=" << running() << ")\n";

        if(_thread_count <= Machine::n_cpus()) { // Only idle is left
            CPU::int_disable();
            if(Machine::cpu_id() == 0) {
                db<Thread>(WRN) << "The last thread has exited!\n";
                if(reboot) {
                    db<Thread>(WRN) << "Rebooting the machine ...\n";
                    Machine::reboot();
                } else
                    db<Thread>(WRN) << "Halting the machine ...\n";
            }
            CPU::halt();
        } else {
            if(_scheduler.schedulables() > 0)
                yield();
            else {
                CPU::int_enable();
                CPU::halt();
            }
        }
    }

    return 0;
}


// Id forwarder to the spin lock
unsigned int This_Thread::id() 
{ 
    return _not_booting ?
        reinterpret_cast<unsigned int>(Thread::self()) : Machine::cpu_id() + 1;
}

__END_SYS
