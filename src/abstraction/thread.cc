// EPOS-- Thread Abstraction Implementation

#include <thread.h>
#include <mmu.h>

__BEGIN_SYS

// Class attributes
Thread * volatile Thread::_running;
Thread::Queue Thread::_ready;
Thread::Queue Thread::_suspended;

// Methods
int Thread::join() {
    db<Thread>(TRC) << "Thread::join(this=" << this
		    << ",state=" << _state << ")\n";

    while(_state != FINISHING)
	yield();

    return *((int *)_stack);
}

void Thread::pass() {
    db<Thread>(TRC) << "Thread::pass(this=" << this << ")\n";

    prevent_scheduling();

    Thread * old = _running;
    old->_state = READY;
    _ready.insert(&old->_link);

    _ready.remove(this);
    _state = RUNNING;
    _running = this;

//     old->_context->save(); // can be used to force an update
    db<Thread>(INF) << "old={" << old << "," 
		    << *old->_context << "}\n";
    db<Thread>(INF) << "new={" << _running << "," 
		    << *_running->_context << "}\n";
	
    CPU::switch_context(&old->_context, _context);

    allow_scheduling();
}

void  Thread::suspend()
{
    db<Thread>(TRC) << "Thread::suspend(this=" << this << ")\n";

    prevent_scheduling();

    _state = SUSPENDED;
    _suspended.insert(&_link);

    if(this == _running) {
	if(!_ready.empty()) {
	    _running = _ready.remove()->object();
	    _running->_state = RUNNING;

//          _context->save(); // can be used to force an update
	    db<Thread>(INF) << "old={" << this << "," 
			    << *_context << "}\n";
	    db<Thread>(INF) << "new={" << _running << "," 
			    << *_running->_context << "}\n";

	    CPU::switch_context(&_context, _running->_context);
	} else
	    idle(); // implicitly reenables scheduling
    } else
	_ready.remove(this);

    allow_scheduling();
}	    

void  Thread::resume() {
    db<Thread>(TRC) << "Thread::resume(this=" << this << ")\n";

    if(_state != SUSPENDED) 
	return;

    prevent_scheduling();

    if(_suspended.remove(this)) {
	_state = READY;
	_ready.insert(&_link);
    } else // the thread has terminated while suspended (e.g. by delete)
	db<Thread>(WRN) << "Thread::resume called with defunct thread!\n";
    
    allow_scheduling();

    if(Traits::preemptive)
	reschedule();
}

void Thread::yield() {
    db<Thread>(TRC) << "Thread::yield()\n";

    prevent_scheduling();

    if(!_ready.empty()) {
	Thread * old = _running;
	old->_state = READY;
	_ready.insert(&old->_link);

	_running = _ready.remove()->object();
	_running->_state = RUNNING;

// 	old->_context->save(); // can be used to force an update
	db<Thread>(INF) << "old={" << old << "," 
			<< *old->_context << "}\n";
	db<Thread>(INF) << "new={" << _running << "," 
			<< *_running->_context << "}\n";
	
	if(_running != old)
	    CPU::switch_context(&old->_context, _running->_context);
    }

    allow_scheduling();
}

void Thread::exit(int status)
{
    db<Thread>(TRC) << "Thread::exit(status=" << status << ")\n";

    prevent_scheduling();

    if(_ready.empty() && !_suspended.empty())
	idle(); // implicitly reenables scheduling
    
    if(!_ready.empty()) {
	Thread * old = _running;
	old->_state = FINISHING;
	*((int *)(void *)old->_stack) = status;

	_running = _ready.remove()->object();
	_running->_state = RUNNING;

// 	old->_context->save(); // can be used to force an update
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

    allow_scheduling();
}

void Thread::idle()
{
    db<Thread>(TRC) << "Thread::idle()\n";

    db<Thread>(WRN) << "There are no runnable threads at the moment!\n";
    db<Thread>(WRN) << "Halting the CPU ...\n";

    allow_scheduling();
    CPU::halt();
}

__END_SYS
