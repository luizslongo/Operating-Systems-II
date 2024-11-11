/**
Testes de deadline de threads periódicas e aperiódicas.

O objetivo desse teste é demonstrar situações onde o nosso algoritmo não é capaz
de impedir a perda de deadline.

 */

#include <architecture/cpu.h>
#include <real-time.h>
#include <scheduler.h>
#include <utility/ostream.h>
#include <utility/wrapped_ostream.h>

using namespace EPOS;

OStream cout;

const unsigned int iterations = 10;
const Milisecond period_a = 150;
const Milisecond period_c = 200;
const Milisecond period_e = 250;
const Milisecond period_g = 200;
const Milisecond wcet_a = 150;
const Milisecond wcet_c = 150;
const Milisecond wcet_e = 150;
const Milisecond wcet_g = 150;

int my_func(char id, int limit) {
//   _ostream_lock.acquire();
  cout << '<' << CPU::id() << "> BEGIN " << id << '\n';
//   _ostream_lock.release();

  do {
    long long b = 0;
    for (int i = 0; i < limit; ++i) {
      b += i - i / 3;
    }
    // _ostream_lock.acquire();
    cout << '<' << CPU::id() << "> Finished Iteration: " << id << '\n';
    // _ostream_lock.release();
  } while (Periodic_Thread::wait_next());

//   _ostream_lock.acquire();
  cout << '<' << CPU::id() << "> END " << id << '\n';
//   _ostream_lock.release();
  return 0;
}
int my_func_aperiodic(char id) {
//   _ostream_lock.acquire();
  cout << '<' << CPU::id() << "> BEGIN " << id << '\n';
//   _ostream_lock.release();
  long long b = 0;
  for (int i = 0; i < 1e1; ++i) {
    b += i - i / 3;
  }
//   _ostream_lock.acquire();
  cout << '<' << CPU::id() << "> END " << id << '\n';
//   _ostream_lock.release();

  return 0;
}

int main() { //                                       p,               d, c,
             //                                       act, t
  Periodic_Thread *a = new Periodic_Thread(
      RTConf(period_a * 1000, period_a * 1000, wcet_a * 1000, 0, iterations,
             EDF_Modified::CRITICAL),
      &my_func, 'A', (int)1e8);
  Periodic_Thread *b = new Periodic_Thread(
      RTConf(period_c * 1000, period_c * 1000, wcet_c * 1000, 0, iterations,
             EDF_Modified::BEST_EFFORT),
      &my_func, 'B', (int)3e8);
  Periodic_Thread *c = new Periodic_Thread(
      RTConf(period_e * 1000, period_e * 1000, wcet_e * 1000, 0, iterations,
             EDF_Modified::BEST_EFFORT),
      &my_func, 'C', (int)1e8);
  Periodic_Thread *d = new Periodic_Thread(
      RTConf(period_g * 1000, period_g * 1000, wcet_g * 1000, 0, iterations,
             EDF_Modified::BEST_EFFORT),
      &my_func, 'D', (int)1e8);
  Thread *h = new Thread(&my_func_aperiodic, 'H');
  Thread *e = new Thread(&my_func_aperiodic, 'E');
  Thread *f = new Thread(&my_func_aperiodic, 'F');
  Thread *g = new Thread(&my_func_aperiodic, 'G');

  a->join();
  b->join();
  c->join();
  d->join();
  h->join();
  e->join();
  g->join();
  f->join();

  cout << "DEADLINES MISSED: A -> "
       << a->criterion().statistics().deadlines_missed << ", B-> "
       << b->criterion().statistics().deadlines_missed << ", C -> "
       << c->criterion().statistics().deadlines_missed << ", D -> "
       << d->criterion().statistics().deadlines_missed << '\n';
  
  return 0;
}