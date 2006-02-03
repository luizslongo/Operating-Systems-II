// EPOS-- Global System Abstraction Implementation

#include <utility/ostream.h>
#include <utility/heap.h>
#include <machine.h>
#include <system.h>

__BEGIN_SYS

// Global objects
OStream kout;
OStream kerr;

// Class attributes
System_Info<Machine> * System::_si =
    reinterpret_cast<System_Info<Machine> *>(Memory_Map<Machine>::SYS_INFO);

Heap System::_heap;

__END_SYS

