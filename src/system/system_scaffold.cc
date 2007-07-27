// EPOS-- System Scaffold and System Abstraction Implementation

#include <utility/ostream.h>
#include <utility/heap.h>
#include <machine.h>
#include <thread.h>
#include <system.h>

// LIBC Heritage

__USING_SYS

extern "C" { 
    void _exit(int s) {
	Thread::exit(s); for(;;);
    }

    void __cxa_pure_virtual() { 
	db<void>(ERR) << "__cxa_pure_virtual() called!\n"; 
	Machine::panic();
    }
}

__BEGIN_SYS

// Global objects
OStream kout;
OStream kerr;

// System class attributes
System_Info<Machine> * System::_si =
    reinterpret_cast<System_Info<Machine> *>(Memory_Map<Machine>::SYS_INFO);

Heap System::_heap;

__END_SYS
