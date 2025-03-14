// EPOS Application Initializer

#include <architecture.h>
#include <utility/heap.h>
#include <machine.h>
#include <system.h>

extern "C" char _end; // defined by GCC

__BEGIN_SYS

class Init_Application
{
private:
    static const unsigned int HEAP_SIZE = Traits<Application>::HEAP_SIZE;
    static const unsigned int STACK_SIZE = Traits<Application>::STACK_SIZE;

public:
    Init_Application() {
        db<Init>(TRC) << "Init_Application()" << endl;

        // Only the bootstrap CPU runs INIT_APPLICATION
        CPU::smp_barrier();
        if(CPU::id() != CPU::BSP) {
            CPU::smp_barrier();
            return;
        }

        // Initialize Application's heap
        db<Init>(INF) << "Initializing application's heap: ";
        if(Traits<System>::multiheap) { // heap in data segment arranged by SETUP
            db<Init>(INF) << endl;
            char * heap = (MMU::align_page(&_end) >= CPU::Log_Addr(Memory_Map::APP_DATA)) ? MMU::align_page(&_end) : CPU::Log_Addr(Memory_Map::APP_DATA); // ld is eliminating the data segment in some compilations, particularly for RISC-V, and placing _end in the code segment
            if(Traits<Build>::SMOD != Traits<Build>::KERNEL)
                heap += MMU::align_page(Traits<Application>::STACK_SIZE);
            Application::_heap = new (&Application::_preheap[0]) Application_Heap(heap, HEAP_SIZE);
            // foi.
            // finalmente. Agora fazer o scheduler
            // Sim. N aguento mais esse EPOS. Qual critério vamos usar? Pra separar as tarefas?
            // Menos threads? 
            // Pode ser. Vamo no mais simples mesmo pra terminar logo.
            // só temos que descobrir como saber o número de threads pelo PEDF
            // 
            // O scheduler provavelmente tem um método pra isso. Maaasss, o mais simples vai ser manter um contador pra cada CPU indicando as threads que tão nela.
            // tipo o t
            // Vai ser algo bem semelhante aquilo que mencionei pro luiz no discord.
            
            // Ai incrementamos o contador quando uma thread é criada, no construtor provavelmente. E decrementamos no destrutor ou no método exit() da thread.
            // Como esses dois acontecem atomicamente, ta safe.
        } else {
            db<Init>(INF) << "adding all free memory to the unified system's heap!" << endl;
            for(unsigned int frames = MMU::allocable(); frames; frames = MMU::allocable())
                System::_heap->free(MMU::alloc(frames), frames * sizeof(MMU::Page));
        }

        CPU::smp_barrier(); // complete the barrier on line 25
    }
};

// Global object "init_application"  must be linked to the application (not to the system) and there constructed at first.
Init_Application init_application;

__END_SYS
