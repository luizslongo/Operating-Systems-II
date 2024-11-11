/**
Testes de deadline de threads periódicas e aperiódicas.

O objetivo desse teste é demonstrar situações onde o nosso algoritmo funciona corretamente,
sem perder muitas deadlines, causar starvation ou 

 */

#include <utility/ostream.h>
#include <real-time.h>
#include <architecture/cpu.h>
#include <scheduler.h>

using namespace EPOS;

OStream cout;

const unsigned int iterations = 10;
const Milisecond period_a = 150;
const Milisecond period_c = 183;
const Milisecond period_e = 267;
const Milisecond period_g = 140;
const Milisecond wcet_a = 50;
const Milisecond wcet_c = 20;
const Milisecond wcet_e = 60;
const Milisecond wcet_g = 40;


int my_func(char id, int limit) {
  _ostream_lock.acquire();
  cout << '<' << CPU::id() << "> BEGIN " << id << '\n';
  _ostream_lock.release();
  static int exec_count[8] = {0};

  do {
    long long b = 0;
    for (int i = 0; i < limit; ++i) {
      b += i - i / 3;
    }
    exec_count[id - 'A']++;
    _ostream_lock.acquire();
    cout << "Thread " << id << " completed iteration. Total executions: " << exec_count[id - 'A'] << '\n';
    _ostream_lock.release();
  } while (Periodic_Thread::wait_next());

  _ostream_lock.acquire();
  cout << '<' << CPU::id() << "> END " << id << '\n';
  _ostream_lock.release();
  return 0;
}

int my_func_aperiodic(char id) {
  _ostream_lock.acquire();
  cout << '<' << CPU::id() << "> BEGIN " << id << '\n';
  _ostream_lock.release();
  long long b = 0;
  for (int i = 0; i < 1e1; ++i) {
    b += i - i / 3;
  }
  _ostream_lock.acquire();
  cout << '<' << CPU::id() << "> END " << id << '\n';
  _ostream_lock.release();

  return 0;
}

int main()
{
    Periodic_Thread* a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, wcet_a * 1000, 0, iterations, EDF_Modified::CRITICAL)   , &my_func, 'A', (int)1e6);
    Thread* b = new Thread(&my_func_aperiodic, 'B');
    Periodic_Thread* c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, wcet_c * 1000, 0, iterations, EDF_Modified::BEST_EFFORT), &my_func, 'C', (int)1e6);
    Thread* d = new Thread(&my_func_aperiodic, 'D');
    Periodic_Thread* e = new Periodic_Thread(RTConf(period_e * 1000, period_e * 1000, wcet_e * 1000, 0, iterations, EDF_Modified::CRITICAL)   , &my_func, 'E', (int)1e6);
    Thread* f = new Thread(&my_func_aperiodic, 'F');
    Periodic_Thread* g = new Periodic_Thread(RTConf(period_g * 1000, period_g * 1000, wcet_g * 1000, 0, iterations, EDF_Modified::BEST_EFFORT)   , &my_func, 'G', (int)1e6);    
    Thread* h = new Thread(&my_func_aperiodic, 'H');
    
    cout << "A: " << a << " B: " << b << " C: " << c << " D: " << d << "E: "  << e << "F: " << f << "G: " << g << "H: " << h << endl;

    a->join();
    b->join();
    c->join();
    d->join();
    h->join();
    e->join();
    g->join();
    f->join();

    cout << "DEADLINES MISSED: A -> "
       << a->criterion().statistics().deadlines_missed << ", C -> "
       << c->criterion().statistics().deadlines_missed << ", E -> "
       << e->criterion().statistics().deadlines_missed << ", G -> "
       << g->criterion().statistics().deadlines_missed << '\n';

    // Always 100 for non-periodic.
    cout << "AVERAGE FREQUENCY: A -> "
       << (a->criterion().statistics().average_frequency*100) / CPU::max_clock()
       << "%, B -> "
       << 100
       << "%, C -> "
       << (c->criterion().statistics().average_frequency*100) / CPU::max_clock()
       << "%, D -> "
       << 100
       << "%, E -> "
       << (e->criterion().statistics().average_frequency * 100) / CPU::max_clock()
       << "%, F -> "
       << 100
       << "%, G -> "
       << (g->criterion().statistics().average_frequency * 100) / CPU::max_clock()
       << "%, H -> "
       << 100
       << "%\n";
    return 0;
}