// EPOS System Initialization

#include <system.h>
#include <time.h>
#include <process.h>

__BEGIN_SYS


/*
`c` --> continue
`q` --> quit
`break` --> lugar de parada. Ex: break System::init() (use e abuse de tab-completion)
`info threads` --> printa onde cada thread (cpu) está executando
`backtrace` --> printa call stack das funções
`thread x` --> Muda a execução atual.
`ìnfo registers all` --> printa informações sobre todos os registradores 
`info variables` -> list "All global and static variable names" (huge list).
`info locals` -> list "Local variables of current stack frame" (names and values), including static variables in that function.
`info args` -> list "Arguments of the current stack frame" (names and values).
*/

void System::init()
{
    // These abstractions are initialized only once (by the bootstrap CPU)
    if(CPU::id() == CPU::BSP) {
        if(Traits<Alarm>::enabled)
            Alarm::init();
    }
    
    // Barrier needed here to make sure Alarm is initialized before thread initialization.
    CPU::smp_barrier();

    // These abstractions are initialized by all CPUs
    if(Traits<Thread>::enabled)
        Thread::init();
}

__END_SYS
