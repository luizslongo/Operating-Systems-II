// EPOS-- Thread Abstraction Implementation

#include <system/kmalloc.h>
#include <thread.h>
#include <alarm.h>
#include <machine.h>

__BEGIN_SYS

// Class attributes
unsigned int Thread::_thread_count = 0;
Scheduler<Thread> Thread::_scheduler;

// Methods
void Thread::common_constructor(Log_Addr entry, unsigned int stack_size) 
{
    db<Thread>(TRC) << "Thread(entry=" << (void *)entry 
		    << ",state=" << _state
		    << ",rank=" << _link.rank()
		    << ",stack={b=" << _stack
		    << ",s=" << stack_size
		    << "},context={b=" << _context
		    << "," << *_context << "}) => " << this << "\n";
    
    _thread_count++;
    
    _scheduler.insert(this);
    if((_state != READY) && (_state != RUNNING))
	_scheduler.suspend(this);

    if(preemptive)
	reschedule();

    allow_scheduling();
}

Thread::~Thread()
{
    prevent_scheduling();

    db<Thread>(TRC) << "~Thread(this=" << this 
		    << ",state=" << _state
		    << ",rank=" << _link.rank()
		    << ",stack={b=" << _stack
		    << ",context={b=" << _context
		    << "," << *_context << "})\n";

    switch(_state) {
    case BEGINNING:  _scheduler.resume(this); break;
    case RUNNING: exit(-1); break; // Self deleted itself!
    case READY: break;
    case SUSPENDED: _scheduler.resume(this); break;
    case WAITING: _waiting->remove(this); _scheduler.resume(this); break;
    case FINISHING: break;
    }
    
    _scheduler.remove(this);
    
    allow_scheduling();

    kfree(_stack);
}

void Thread::priority(const Priority & p)
{
    prevent_scheduling();

    db<Thread>(TRC) << "Thread::priority(this=" << this
		    << ",prio=" << p << ")\n";

    _link.rank(int(p));

    if(preemptive)
	reschedule();

    allow_scheduling();
}

int Thread::join()
{
    prevent_scheduling();

    db<Thread>(TRC) << "Thread::join(this=" << this
		    << ",state=" << _state << ")\n";

    if(_state != FINISHING) {
	_joining = running();
	_joining->suspend(); // implicitly allows scheduling
    }

    allow_scheduling();

    return *static_cast<int *>(_stack);
}

void Thread::pass()
{
    prevent_scheduling();

    db<Thread>(TRC) << "Thread::pass(this=" << this << ")\n";

    Thread * prev = running();

    Thread * next = _scheduler.choose(this);
    if(next)
	switch_threads(prev, next);
    else
 	db<Thread>(WRN) << "Thread::pass => thread (" << this 
 			<< ") not ready\n";

    allow_scheduling();
}

void Thread::suspend()
{
    prevent_scheduling();

    db<Thread>(TRC) << "Thread::suspend(this=" << this << ")\n";

    Thread * prev = running();

    _scheduler.suspend(this);
    _state = SUSPENDED;

    Thread * next = running();

    Alarm::reset_master();
    switch_threads(prev, next); // null if this != running() at the begin

    allow_scheduling();
}	    

void Thread::resume()
{
    prevent_scheduling();

    db<Thread>(TRC) << "Thread::resume(this=" << this << ")\n";

    if(_state == SUSPENDED) {
	_state = READY;
	_scheduler.resume(this);
    } else
	db<Thread>(WRN) << "Resume called for unsuspended object!\n";

    if(preemptive)
	reschedule();

    allow_scheduling();
}


// Class methods

void Thread::yield()
{
    prevent_scheduling();

    db<Thread>(TRC) << "Thread::yield(running=" << running() << ")\n";

    Alarm::reset_master();
    switch_threads(running(), _scheduler.choose_another());

    allow_scheduling();
}

void Thread::exit(int status)
{
    prevent_scheduling();

    db<Thread>(TRC) << "Thread::exit(running=" << running() 
		    <<",status=" << status << ")\n";

    Thread * thr = running();
    _scheduler.remove(thr);
    *static_cast<int *>(thr->_stack) = status;
    thr->_state = FINISHING;

    _thread_count--;

    if(thr->_joining) {
	thr->_joining->_state = READY;
	_scheduler.resume(thr->_joining);
	thr->_joining = 0;
    }

    Alarm::reset_master();
    switch_threads(thr, _scheduler.choose());

    allow_scheduling();
}

void Thread::sleep(Queue * q)
{
    prevent_scheduling();

    db<Thread>(TRC) << "Thread::sleep(running=" << running()
		    << ",q=" << q << ")\n";

    Thread * thr = running();

    _scheduler.suspend(thr);
    thr->_state = WAITING;
    q->insert(&thr->_link);
    thr->_waiting = q;

    Alarm::reset_master();
    switch_threads(thr, _scheduler.chosen());
    
    allow_scheduling();
}

void Thread::wakeup(Queue * q) 
{
    prevent_scheduling();

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

    allow_scheduling();
}

void Thread::wakeup_all(Queue * q) 
{
    prevent_scheduling();

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

    allow_scheduling();
}

void Thread::time_reschedule()
{
    // timer invokes the master handler with interrupts enabled!

    prevent_scheduling(); 

    switch_threads(running(), _scheduler.choose());

    // scheduling will be reenabled by switch_threads
}

void Thread::reschedule()
{
    // scheduling must be disabled at this point

    Thread * prev = running();
    Thread * next = _scheduler.choose();

    Alarm::reset_master();
    switch_threads(prev, next);

//     switch_threads(running(), _scheduler.choose());

    // scheduling will be reenabled by switch_threads
}

void Thread::implicit_exit() 
{
    exit(CPU::fr()); 
}

int Thread::idle()
{
    while(true) {
	db<Thread>(TRC) << "Thread::idle()\n";

	if(_thread_count <= 1) { 
	    db<Thread>(WRN) << "The last thread has exited!\n";
	    db<Thread>(WRN) << "Halting the CPU ...\n";
	    CPU::int_disable();
	}

 	CPU::halt();

	if(_scheduler.schedulables() > 1)
            yield();
    }

    return 0;
}

__END_SYS
