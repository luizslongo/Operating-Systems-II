// EPOS-- System Scaffold

#include <utility/ostream.h>
#include <utility/debug.h>
#include <utility/heap.h>
#include <machine.h>
#include <thread.h>

__BEGIN_SYS

// Global constructros aren't called for system in built-in mode!
// So don't use them here!

System_Info * si =
    reinterpret_cast<System_Info *>(Memory_Map<Machine>::SYS_INFO);

char _sys_heap[Traits<Machine>::SYSTEM_HEAP_SIZE];
Heap sys_heap(_sys_heap, Traits<Machine>::SYSTEM_HEAP_SIZE);

Heap app_heap; // sure this should be here????

OStream kout, kerr;

__END_SYS

__USING_SYS

// LIBC Heritage
extern "C" { 
    void _exit(int s) { Thread::exit(s); for(;;); }

    void __cxa_pure_virtual() { 
	db<void>(ERR) << "__cxa_pure_virtual() called!\n"; 
	Machine::panic();
    }
}

