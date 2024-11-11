/*
Testes permissivos de threads aperiódicas.
Essencialmente, o objetivo destes testes é mostrar que a execução de threads aperiódicas funciona como esperado.
*/

#include "utility/wrapped_ostream.h"
#include <utility/ostream.h>
#include <real-time.h>
#include <architecture/cpu.h>
#include <scheduler.h>

using namespace EPOS;

OStream cout;

int my_func_aperiodic(char id) {
    _ostream_lock.acquire();
    cout << '<' << CPU::id() << "> BEGIN " << id << '\n';
    _ostream_lock.release();
    long long b = 0;

    for (int i = 0; i < 1e7; ++i) {
       b += i - i/3;
    }
    _ostream_lock.acquire();
    cout << '<' << CPU::id() << "> END " << id << '\n';
    _ostream_lock.release();
    return 0;
}

int main()
{   
    Thread* a = new Thread(&my_func_aperiodic, 'A');
    Thread* b = new Thread(&my_func_aperiodic, 'B');
    Thread* c = new Thread(&my_func_aperiodic, 'C');
    Thread* d = new Thread(&my_func_aperiodic, 'D');
    Thread* e = new Thread(&my_func_aperiodic, 'E');
    Thread* f = new Thread(&my_func_aperiodic, 'F');
    Thread* g = new Thread(&my_func_aperiodic, 'G');
    Thread* h = new Thread(&my_func_aperiodic, 'H');
    
    _ostream_lock.acquire();
    cout << "A: " << a << " B: " << b << " C: " << c << " D: " << d << "E: "  << e << "F: " << f << "G: " << g << "H: " << h << endl;
    _ostream_lock.release();

    a->join();
    b->join();
    c->join();
    d->join();
    h->join();
    e->join();
    g->join();
    f->join();

    
    return 0;
}