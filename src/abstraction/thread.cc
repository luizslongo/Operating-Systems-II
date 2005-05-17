// EPOS-- Thread Abstraction Implementation

#include <thread.h>
#include <mmu.h>

__BEGIN_SYS

// Class attributes

Thread * volatile Thread::_running;
Thread * Thread::_idle;
Thread::Queue Thread::_ready;
Thread::Queue Thread::_suspended;


// Methods

int Thread::join()
{
    db<Thread>(TRC) << "Thread::join(this=" << this
		    << ",state=" << _state << ")\n";

    prevent_scheduling();

    if(_state != FINISHING) {
	_joining = _running;
	_joining->suspend(); // implicitly allows scheduling
    }

    allow_scheduling();

    return *static_cast<int *>(_stack);
}

void Thread::pass()
{
    db<Thread>(TRC) << "Thread::pass(this=" << this << ")\n";

    prevent_scheduling();

    if(_ready.remove(this)) { // this is ready to receive the CPU
	_running->_state = READY;
	_ready.insert(&_running->_link);
	switch_to(this);
    } else 
	db<Thread>(WRN) << "Thread::pass => thread (" << this 
			<< ") not ready\n";

    allow_scheduling();
}

void Thread::suspend()
{
    db<Thread>(TRC) << "Thread::suspend(this=" << this << ")\n";

    prevent_scheduling();

    _state = SUSPENDED;
    _suspended.insert(&_link);

    if(this == _running)
	switch_to(_ready.remove()->object());
    else
	_ready.remove(this);
    
    allow_scheduling();
}	    

void Thread::resume()
{
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


// Class methods

void Thread::yield()
{
    db<Thread>(TRC) << "Thread::yield()\n";

    prevent_scheduling();

    if(!_ready.empty()) {
	_running->_state = READY;
	_ready.insert(&_running->_link);
	switch_to(_ready.remove()->object());
    }

    allow_scheduling();
}

void Thread::exit(int status)
{
    db<Thread>(TRC) << "Thread::exit(status=" << status << ")\n";

    prevent_scheduling();

    *static_cast<int *>(_running->_stack) = status;
    _running->_state = FINISHING;
    if(_running->_joining) {
	Thread * tmp = _running->_joining;
	_running->_joining = 0;
	tmp->resume(); // implicitly allows scheduling
	prevent_scheduling();
    }

    switch_to(_ready.remove()->object());

    allow_scheduling();
}

void Thread::sleep(Queue * q)
{
    db<Thread>(TRC) << "Thread::sleep(q=" << q << ")\n";

    prevent_scheduling();

    _running->_state = WAITING;
    q->insert(&_running->_link);
    _running->_waiting = q;

    switch_to(_ready.remove()->object());
    
    allow_scheduling();
}

void Thread::wakeup(Queue * q) 
{
    db<Thread>(TRC) << "Thread::wakeup(q=" << q << ")\n";

    prevent_scheduling();

    if(!q->empty()) {
	Thread * t = q->remove()->object();
	t->_state = READY;
	t->_waiting = 0;
	_ready.insert(&t->_link);
    }

    allow_scheduling();

    if(Traits::preemptive)
	reschedule();
}

void Thread::wakeup_all(Queue * q) 
{
    db<Thread>(TRC) << "Thread::wakeup_all(q=" << q << ")\n";

    prevent_scheduling();

    while(!q->empty()) {
	Thread * t = q->remove()->object();
	t->_state = READY;
	t->_waiting = 0;
	_ready.insert(&t->_link);
    }

    allow_scheduling();

    if(Traits::preemptive)
	reschedule();
}

__END_SYS
