// EPOS System Scaffold and System Component Implementation

#include <utility/ostream.h>
#include <utility/heap.h>
#include <machine.h>
#include <memory.h>
#include <system.h>

__BEGIN_SYS

// This class purpose is simply to define a well-known entry point for
// the system. It must be declared as the first global object in
// system_scaffold.cc
class First_Object
{
public:
    First_Object() {
        Machine::pre_init(System::_si);
    }
};

// Global objects
// These objects might be reconstructed several times in SMP configurations,
// so their constructors must be stateless!
First_Object __entry;
OStream kout;
OStream kerr;


// System class attributes
#ifdef __mach_cortex__
    extern "C" { extern unsigned int _sys_info; }
    System_Info * System::_si = reinterpret_cast<System_Info *>(&_sys_info);
#else
    System_Info * System::_si = reinterpret_cast<System_Info *>(Memory_Map::SYS_INFO);
#endif
char System::_preheap[];
Segment * System::_heap_segment;
Heap * System::_heap;

__END_SYS
