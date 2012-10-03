// EPOS Thread Abstraction Implementation

#include <system/kmalloc.h>
#include <machine.h>
#include <thread.h>
#include <scheduler.h>
#include <alarm.h>

__BEGIN_SYS

// Class attributes
Spin Thread::_lock;
Scheduler_Timer * Thread::_timer;

Thread* volatile Thread::_running;
Thread::ThreadQueue Thread::_ready;
Thread::ThreadQueue Thread::_suspended;


// This_Thread class attributes
bool This_Thread::_not_booting;


// Methods
void Thread::common_constructor(Log_Addr entry, unsigned int stack_size) 
{
    db<Thread>(TRC) << "Thread(entry=" << (void *)entry 
		    << ",state=" << _state
		    << ",stack={b=" << _stack
		    << ",s=" << stack_size
		    << "},context={b=" << _context
		    << "," << *_context << "}) => " << this << "\n";

     if(active_scheduler)
        CPU::int_disable();

    switch(_state) {
        case RUNNING: break;
        case SUSPENDED: _suspended.insert(&_link); break;
        default: _ready.insert(&_link);
    }

    if(active_scheduler)
        CPU::int_enable();
}


Thread::~Thread()
{
    db<Thread>(TRC) << "~Thread(this=" << this 
		    << ",state=" << _state
		    << ",stack={b=" << _stack
		    << ",context={b=" << _context
		    << "," << *_context << "})\n";

    _ready.remove(this);
    _suspended.remove(this);

    kfree(_stack);
}


int Thread::join()
{
    db<Thread>(TRC) << "Thread::join(this=" << this
		    << ",state=" << _state << ")\n";

    while(_state != FINISHING)
        yield();


    return *static_cast<int *>(_stack);
}


void Thread::pass()
{
    db<Thread>(TRC) << "Thread::pass(this=" << this << ")\n";

    if(active_scheduler)
        CPU::int_disable();

    Thread* old = _running;
    old->_state = READY;
    _ready.insert(&old->_link);

    _ready.remove(this);
    _state = RUNNING;
    _running = this;

    // old->_context->save(); // can be used to force an update
    db<Thread>(INF) << "old={" << old << ","
            << *old->_context << "}\n";
    db<Thread>(INF) << "new={" << _running << ","
            << *_running->_context << "}\n";

    CPU::switch_context(&old->_context, _context);

    if(active_scheduler)
        CPU::int_enable();
}


void Thread::suspend()
{
    db<Thread>(TRC) << "Thread::suspend(this=" << this << ")\n";

    if(active_scheduler)
        CPU::int_disable();

    if(_running != this)
        _ready.remove(this);

    _state = SUSPENDED;
    _suspended.insert(&_link);

    if(!_ready.empty()) {
        _running = _ready.remove()->object();
        _running->_state = RUNNING;

        //  _context->save(); // can be used to force an update
        db<Thread>(INF) << "old={" << this << ","
                << *_context << "}\n";
        db<Thread>(INF) << "new={" << _running << ","
                << *_running->_context << "}\n";

        CPU::switch_context(&_context, _running->_context);
    } else
        idle(); // implicitly re-enables interrupts

    if(active_scheduler)
        CPU::int_enable();
}


void Thread::resume()
{
    db<Thread>(TRC) << "Thread::resume(this=" << this << ")\n";

   if(active_scheduler)
       CPU::int_disable();

   _suspended.remove(this);
   _state = READY;
   _ready.insert(&_link);

   if(active_scheduler)
       CPU::int_enable();
}


// Class methods
void Thread::yield()
{
    db<Thread>(TRC) << "Thread::yield()\n";
    if(active_scheduler)
        CPU::int_disable();

    if(!_ready.empty()) {
        Thread* old = _running;
        old->_state = READY;
        _ready.insert(&old->_link);

        _running = _ready.remove()->object();
        _running->_state = RUNNING;

        //  old->_context->save(); // can be used to force an update
        db<Thread>(INF) << "old={" << old << ","
                << *old->_context << "}\n";
        db<Thread>(INF) << "new={" << _running << ","
                << *_running->_context << "}\n";

        CPU::switch_context(&old->_context, _running->_context);
    }

    if(active_scheduler)
        CPU::int_enable();
}


void Thread::exit(int status)
{
    db<Thread>(TRC) << "Thread::exit(status=" << status << ")\n";

    if(active_scheduler)
        CPU::int_disable();

    if(_ready.empty() && !_suspended.empty())
        idle(); // implicitly re-enables interrupts

    if(active_scheduler)
        CPU::int_disable();

    if(!_ready.empty()) {
        Thread* old = _running;
        old->_state = FINISHING;
        *((int *)(void *)old->_stack) = status;

        _running = _ready.remove()->object();
        _running->_state = RUNNING;

        //  old->_context->save(); // can be used to force an update
        db<Thread>(INF) << "old={" << old << ","
                << *old->_context << "}\n";
        db<Thread>(INF) << "new={" << _running << ","
                << *_running->_context << "}\n";

        CPU::switch_context(&old->_context, _running->_context);
    } else {
        db<Thread>(WRN) << "The last thread in the system has exited!\n";
        db<Thread>(WRN) << "Halting the CPU ...\n";
        CPU::int_disable();
        CPU::halt(); // this must be turned into a conf-feature (reboot, halt)
    }

    if(active_scheduler)
        CPU::int_enable();
}


void Thread::reschedule()
{
    yield();
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


void Thread::idle()
{
    db<Thread>(TRC) << "Thread::idle()\n";

    db<Thread>(WRN) << "There are no runnable threads at the moment!\n";
    db<Thread>(WRN) << "Halting the CPU ...\n";

    CPU::int_enable();
    CPU::halt();
}


// Id forwarder to the spin lock
unsigned int This_Thread::id() 
{ 
    return _not_booting ?
	reinterpret_cast<unsigned int>(Thread::self()) :
	Machine::cpu_id() + 1;
}

__END_SYS
