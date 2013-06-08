// EPOS System Initializer

#include <machine.h>
#include <system.h>
#include <address_space.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

class Init_System
{
private:
    static const unsigned int HEAP_SIZE = Traits<System>::HEAP_SIZE;

public:
    Init_System() {
        db<Init>(TRC) << "Init_System()" << endl;

        Machine::smp_barrier();

        // Only the boot CPU runs INIT_SYSTEM fully
        if(Machine::cpu_id() != 0) {
            // Wait until the boot CPU has initialized the machine
            Machine::smp_barrier();
            // For IA-32, timer is CPU-local. What about other SMPs?
            Timer::init();
            Machine::smp_barrier();
            return;
        }
        
        // Initialize the processor
        db<Init>(INF) << "Initializing the CPU: " << endl;
        CPU::init();
        db<Init>(INF) << "done!\n\n";

        // If EPOS is a library then adjust the application entry point (that
        // was set by SETUP) based on the ELF SYSTEM+APPLICATION image
        System_Info<Machine> * si = System::info();
        if(!si->lm.has_sys)
            si->lmm.app_entry =
        	reinterpret_cast<unsigned int>(&__epos_library_app_entry);

        // Initialize System's heap
        db<Init>(INF) << "Initializing system's heap: " << endl;
        if(Traits<System>::multiheap) {
            new (&System::_heap_segment) Segment(HEAP_SIZE);
            new (&System::_heap) Heap(
                Address_Space(SELF).attach(System::_heap_segment, Memory_Map<Machine>::SYS_HEAP),
                System::_heap_segment.size());
        } else {
            new (&System::_heap) Heap(MMU::alloc(MMU::pages(HEAP_SIZE)), HEAP_SIZE);
        }
        db<Init>(INF) << "done!" << endl;

        // Initialize the machine
        db<Init>(INF) << "Initializing the machine: " << endl;
        Machine::init();
        db<Init>(INF) << "done!" << endl;

        Machine::smp_barrier(); // signalizes "machine ready" to other CPUs
        Machine::smp_barrier(); // wait for them to finish Machine::init()

        // Initialize system abstractions 
        db<Init>(INF) << "Initializing system abstractions: " << endl;
        System::init();
        db<Init>(INF) << "done!" << endl;

        // Initialization continues at init_first
    }
};

// Global object "init_system" must be constructed first.
Init_System init_system;

__END_SYS
