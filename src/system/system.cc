// EPOS-- System Scaffold

#include <utility/ostream.h>
#include <utility/heap.h>
#include <thread.h>

extern "C" { void _exit(int s) { __SYS(Thread)::exit(s); for(;;); }}

__BEGIN_SYS

System_Info * si = reinterpret_cast<System_Info *>(
    Memory_Map<Machine>::SYS_INFO);

OStream kout, kerr;

Heap app_heap;
Heap sys_heap;
char _sys_heap[Traits<Machine>::SYSTEM_HEAP_SIZE];

__END_SYS


