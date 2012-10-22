// EPOS Application Scaffold and Application Abstraction Implementation

#include <utility/ostream.h>
#include <application.h>

__BEGIN_SYS

// Global objects
OStream cout;
OStream cerr;

// Application class attributes
Heap Application::_heap[Traits<Machine>::N_HEAPS];

__END_SYS
