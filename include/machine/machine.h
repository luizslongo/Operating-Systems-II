// EPOS Machine Mediator Common Package

#ifndef __machine_machine_h
#define __machine_machine_h

#include <architecture/cpu.h>

__BEGIN_SYS

class Machine_Common
{
public:
    template<typename Family, int unit = 0>
    struct Initializer
    {
        typedef typename Traits<Family>::DEVICES::template Get<unit>::Result DEV;

        static void init() {
            if(Traits<DEV>::enabled)
                DEV::init(unit);

            Initializer<Family, unit + 1>::init();
        };
    };

    template<typename Family>
    struct Initializer<Family, Traits<Family>::DEVICES::Length>
    {
        static void init() {};
    };

protected:
    Machine_Common() {}

public:
    static void smp_barrier(unsigned long cores) {
        static volatile unsigned long ready[2];
        static volatile unsigned long i;

        if(cores > 1) {
            int j = i;

            CPU::finc(ready[j]);

            if(CPU::id() == 0) {
                while(ready[j] < cores);  // wait for all CPUs to be ready
                i = !i;                   // toggle ready
                ready[j] = 0;             // signalizes waiting CPUs
            } else
                while(ready[j]);          // wait for CPU[0] signal
        }
    }
};

__END_SYS

#endif

#if defined(__MACH_H) && !defined(__common_only__)
#include __MACH_H
#endif
