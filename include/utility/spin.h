// EPOS-- Spin Lock Utility Declarations

#ifndef __spin_h
#define __spin_h

#include <cpu.h>

__BEGIN_SYS

// Forwarder to the running thread id
namespace This_Thread
{
    unsigned int id();
}

// Recursive Spin Lock
class Spin
{
public:
    Spin(): _level(0), _owner(-1) {}

    void acquire() {
	int id = This_Thread::id();

	while(CPU::cas(_owner, -1, id));
	_level++;
    }

    void release() {
    	if(!--_level) _owner = -1;
    }

private:
    volatile unsigned int _level;
    volatile int _owner;
};

__END_SYS

#endif
