/**
Testes relacionados à consumo de energia em cenários onde há threads periódicas
e aperiódicas.

Aqui, dizemos que o consumo de energia é alto quando a frequência média também
é.
 */

#include "architecture/ia32/ia32_cpu.h"
#include "utility/wrapped_ostream.h"
#include <architecture/cpu.h>
#include <real-time.h>
#include <scheduler.h>
#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

const unsigned int iterations = 10;
const Milisecond period_a = 100;
const Milisecond period_c = 200;
const Milisecond period_e = 100;
const Milisecond period_g = 200;
const Milisecond wcet_a = 50;
const Milisecond wcet_c = 20;
const Milisecond wcet_e = 60;
const Milisecond wcet_g = 40;

int my_func(char id, int limit) {
_ostream_lock.acquire();
cout << '<' << CPU::id() << "> BEGIN " << id << '\n';
_ostream_lock.release();
  do {
    long long b = 0;
    for (int i = 0; i < limit; ++i) {
      b += i - i / 3;
    }
    
    _ostream_lock.acquire();
    cout << "Finished Iteration: " << id << '\n';
    _ostream_lock.release();
  } while (Periodic_Thread::wait_next());

    _ostream_lock.acquire();
    cout << '<' << CPU::id() << "> END " << id << '\n';
    _ostream_lock.release();
  return 0;
}
int my_func_aperiodic(char id) {
  long long b = 0;
  for (int i = 0; i < 1e1; ++i) {
    b += i - i / 3;
  }
  OStream cout;
  cout << b << '\n';

  return 0;
}

int main() {
  Periodic_Thread *a = new Periodic_Thread(
      RTConf(period_a * 1000, period_a * 1000, wcet_a * 1000, 0, iterations,
             EDF_Modified::CRITICAL),
      &my_func, 'A', (int)1e6);
  Thread *e = new Thread(&my_func_aperiodic, 'E');
  Periodic_Thread *b = new Periodic_Thread(
      RTConf(period_c * 1000, period_c * 1000, wcet_c * 1000, 0, iterations,
             EDF_Modified::CRITICAL),
      &my_func, 'B', (int)1e7);
  Thread *f = new Thread(&my_func_aperiodic, 'F');
  Periodic_Thread *c = new Periodic_Thread(
      RTConf(period_e * 1000, period_e * 1000, wcet_e * 1000, 0, iterations,
             EDF_Modified::CRITICAL),
      &my_func, 'C', (int)2.5e7);
  Thread *g = new Thread(&my_func_aperiodic, 'G');
  Periodic_Thread *d = new Periodic_Thread(
      RTConf(period_g * 1000, period_g * 1000, wcet_g * 1000, 0, iterations,
             EDF_Modified::CRITICAL),
      &my_func, 'D', (int)1e7);
  Thread *h = new Thread(&my_func_aperiodic, 'H');
  
  a->join();
  b->join();
  c->join();
  d->join();
  h->join();
  e->join();
  g->join();
  f->join();

  cout << "AVERAGE FREQUENCY: A-> "
       << (double)a->criterion().statistics().average_frequency / CPU::max_clock() * 100
       << "%, B -> "
       << (double)b->criterion().statistics().average_frequency / CPU::max_clock() * 100
       << "%, C-> "
       << (double)c->criterion().statistics().average_frequency / CPU::max_clock() * 100
       << "%, D -> "
       << (double)d->criterion().statistics().average_frequency / CPU::max_clock() * 100
       // Always 100 for non-periodic
       << "%, E -> "
       << 100
       << "%, F -> "
       << 100
       << "%, G -> "
       << 100
       << "%, H -> "
       << 100
       << '\n';
  return 0;
}