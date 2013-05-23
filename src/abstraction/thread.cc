// EPOS Thread Abstraction Implementation

#include <machine.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

// Class attributes
volatile unsigned int Thread::_thread_count;
Scheduler_Timer * Thread::_timer;

Thread* volatile Thread::_running;
Thread::Queue Thread::_ready;
Thread::Queue Thread::_suspended;


// This_Thread class attributes
bool This_Thread::_not_booting;


// Methods
void Thread::common_constructor(Log_Addr entry, unsigned int stack_size) 
{
    db<Thread>(TRC) << "Thread(entry=" << entry
                    << ",state=" << _state
                    << ",priority=" << _link.rank()
                    << ",stack={b=" << _stack
                    << ",s=" << stack_size
                    << "},context={b=" << _context
                    << "," << *_context << "}) => " << this << "\n";

    _thread_count++;

    switch(_state) {
        case RUNNING: break;
        case SUSPENDED: _suspended.insert(&_link); break;
        default: _ready.insert(&_link);
    }

    unlock();
}


Thread::~Thread()
{
    lock();

    db<Thread>(TRC) << "~Thread(this=" << this 
                    << ",state=" << _state
                    << ",priority=" << _link.rank()
                    << ",stack={b=" << _stack
                    << ",context={b=" << _context
                    << "," << *_context << "})\n";

    switch(_state) {
    case RUNNING:  // Self deleted itself!
        exit(-1);
        break;
    case READY:
        _ready.remove(this);
        _thread_count--;
        break;
    case SUSPENDED:
        _suspended.remove(this);
        _thread_count--;
        break;
    case WAITING:
        _waiting->remove(this);
        _thread_count--;
        break;
    case FINISHING: // Already called exit()
        break;
    }

    unlock();

    delete _stack;
}


void Thread::priority(const Priority & p)
{
    lock();

    db<Thread>(TRC) << "Thread::priority(this=" << this
                    << ",prio=" << p << ")\n";

    _ready.remove(this);
    _link.rank(int(p));
    _ready.insert(&_link);

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
        _joining->suspend();
    }

    return *reinterpret_cast<int *>(_stack);
}


void Thread::pass()
{
    lock();

    db<Thread>(TRC) << "Thread::pass(this=" << this << ")\n";

    Thread * prev = _running;
    prev->_state = READY;
    _ready.insert(&prev->_link);

    _ready.remove(this);
    _state = RUNNING;
    _running = this;

    dispatch(prev, this);

    unlock();
}


void Thread::suspend()
{
    lock();

    db<Thread>(TRC) << "Thread::suspend(this=" << this << ")\n";

    if(_running != this)
        _ready.remove(this);

    _state = SUSPENDED;
    _suspended.insert(&_link);

    if(_running == this) {
        _running = _ready.remove()->object();
        _running->_state = RUNNING;

        dispatch(this, _running);
    }

    unlock();
}


void Thread::resume()
{
    lock();

    db<Thread>(TRC) << "Thread::resume(this=" << this << ")\n";

   _suspended.remove(this);
   _state = READY;
   _ready.insert(&_link);

   unlock();
}


// Class methods
void Thread::yield()
{
    lock();

    db<Thread>(TRC) << "Thread::yield(running=" << running() << ")\n";

    Thread * prev = _running;
    prev->_state = READY;
    _ready.insert(&prev->_link);

    _running = _ready.remove()->object();
    _running->_state = RUNNING;

    dispatch(prev, _running);

    unlock();
}


void Thread::exit(int status)
{
    lock();

    db<Thread>(TRC) << "Thread::exit(running=" << _running
                    <<",status=" << status << ")\n";

    Thread * prev = _running;
    prev->_state = FINISHING;
   *reinterpret_cast<int *>(prev->_stack) = status;
    prev->_state = FINISHING;

    _thread_count--;

    if(prev->_joining) {
        prev->_joining->resume();
        prev->_joining = 0;
    }

    lock();

    _running = _ready.remove()->object();
    _running->_state = RUNNING;

    dispatch(prev, _running);

    unlock();
}


void Thread::sleep(Queue * q)
{
    db<Thread>(TRC) << "Thread::sleep(running=" << running()
                    << ",q=" << q << ")\n";

    Thread * prev = running();
    prev->_state = WAITING;
    prev->_waiting = q;
    q->insert(&prev->_link);

    _running = _ready.remove()->object();
    _running->_state = RUNNING;

    dispatch(prev, _running);
}


void Thread::wakeup(Queue * q)
{
    db<Thread>(TRC) << "Thread::wakeup(running=" << running()
                    << ",q=" << q << ")\n";

    if(!q->empty()) {
        Thread * t = q->remove()->object();
        t->_state = READY;
        t->_waiting = 0;
        _ready.insert(&t->_link);
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
        _ready.insert(&t->_link);
    }

    if(preemptive)
        reschedule();
    else
        unlock();
}


void Thread::reschedule()
{
    yield();
}


void Thread::implicit_exit() 
{
    exit(CPU::fr()); 
}


int Thread::idle()
{
    while(true) {
        if(Traits<Thread>::trace_idle)
            db<Thread>(TRC) << "Thread::idle()\n";

        if(_thread_count <= 1) { // Only idle is left
            CPU::int_disable();
            db<Thread>(WRN) << "The last thread has exited!\n";
            if(reboot) {
                db<Thread>(WRN) << "Rebooting the machine ...\n";
                Machine::reboot();
            } else {
                db<Thread>(WRN) << "Halting the machine ...\n";
                CPU::halt();
            }
        } else {
            CPU::int_enable();
            CPU::halt();
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
