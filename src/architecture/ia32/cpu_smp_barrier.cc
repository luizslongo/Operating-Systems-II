// EPOS IA32 CPU SMP Barrier Implementation

#include <architecture/ia32/cpu.h>

__BEGIN_SYS

void CPU::smp_barrier(unsigned long n_cpus)
{
    static volatile unsigned long ready[2];
    static volatile unsigned long i;

    if(smp) {
        int j = i;

        finc(ready[j]);
        if(id() == 0) {
            while(ready[j] < n_cpus); // wait for all CPUs to be ready
            i = !i;                   // toggle ready
            ready[j] = 0;             // signalizes waiting CPUs
        } else {
            while(ready[j]);          // wait for CPU[0] signal
        }
    }
}

__END_SYS
