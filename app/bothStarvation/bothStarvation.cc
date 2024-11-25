/**
Testes de deadline de threads periódicas e aperiódicas.

O objetivo desse teste é demonstrar situações onde o nosso algoritmo não é capaz
de impedir o starvation.

 */

#include <utility/ostream.h>
#include <real-time.h>
#include <architecture/cpu.h>
#include <scheduler.h>

using namespace EPOS;

OStream cout;

const unsigned int iterations = 10;
const Milisecond period_a = 100;
const Milisecond period_b = 100;
const Milisecond period_g = 200;
const Milisecond period_h = 200;
const Milisecond wcet_a = 50;
const Milisecond wcet_b = 20;
const Milisecond wcet_g = 60;
const Milisecond wcet_h = 40;

void print_PMU() {
  cout << '<' << CPU::id() << "> LLC_REFERENCES             : " << PMU::read(3)
       << '\n';
  cout << '<' << CPU::id() << "> LLC_MISSES                 : " << PMU::read(4)
       << '\n';
  cout << '<' << CPU::id() << "> BRANCH_INSTRUCTIONS_RETIRED: " << PMU::read(5)
       << '\n';
  cout << '<' << CPU::id() << "> BRANCH_MISSES_RETIRED      : " << PMU::read(6)
       << '\n';
  cout << '<' << CPU::id() << "> UNHALTED_REFERENCE_CYCLES  : " << PMU::read(0)
       << '\n';
  cout << '<' << CPU::id()
       << "> TOTAL_CORE_CYCLES          : " << TSC::time_stamp() << '\n';
}

static int exec_count[8][4];
int my_func(char id, int limit) {
  cout << '<' << CPU::id() << "> BEGIN " << id << '\n';
  // print_PMU();
  do {
    long long b = 0;
    for (int i = 0; i < limit; ++i) {
      b += i - i / 3;
    }
    if (b % 2 == 0) {
      // cout << "AAA\n";
    } else {
      // cout << "BBB\n";
    }
    exec_count[id - 'A'][CPU::id()]++;
    // cout << "Thread " << id << " completed iteration. Total executions: " <<
    // exec_count[id - 'A'] << '\n';
  } while (Periodic_Thread::wait_next());

  cout << '<' << CPU::id() << "> END " << id << '\n';
  // print_PMU();
  return 0;
}

int my_func_aperiodic(char id) {
  cout << '<' << CPU::id() << "> BEGIN " << id << '\n';
  // print_PMU();
  long long b = 0;
  for (int i = 0; i < 1e1; ++i) {
    b += i - i / 3;
  }
  cout << '<' << CPU::id() << "> END " << id << '\n';
  // print_PMU();
  exec_count[id - 'A'][CPU::id()]++;

  return 0;
}

int main()
{   
    Periodic_Thread* a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, wcet_a * 1000, 0, iterations, EDF_Modified::CRITICAL)   , &my_func, 'A', (int)1e8);
    Periodic_Thread* b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, wcet_b * 1000, 0, iterations, EDF_Modified::CRITICAL), &my_func, 'B', (int)1e7);
    Thread* c = new Thread(&my_func_aperiodic, 'C');
    Thread* d = new Thread(&my_func_aperiodic, 'D');
    Thread* e = new Thread(&my_func_aperiodic, 'E');
    Thread* f = new Thread(&my_func_aperiodic, 'F');
    Periodic_Thread* g = new Periodic_Thread(RTConf(period_g * 1000, period_g * 1000, wcet_g * 1000, 0, iterations, EDF_Modified::BEST_EFFORT)   , &my_func, 'G', (int)2.5e7);
    Periodic_Thread* h = new Periodic_Thread(RTConf(period_h * 1000, period_h * 1000, wcet_h * 1000, 0, iterations, EDF_Modified::BEST_EFFORT)   , &my_func, 'H', (int)1e7);    
        
    cout << "A: " << a << " B: " << b << " C: " << c << " D: " << d << "E: "  << e << "F: " << f << "G: " << g << "H: " << h << endl;

    a->join();
    b->join();
    c->join();
    d->join();
    h->join();
    e->join();
    g->join();
    f->join();

    cout << '<' << CPU::id() << "> FINISHING MAIN               " << '\n';
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 4; ++j)
        cout << (char)('A' + i) << "(" << j <<  ") => " << exec_count[i][j] << '\n';
    for (int j = 0; j < 4; ++j) {
      int tot = 0;
      for (int i = 0; i < 8; ++i)
        tot += exec_count[i][j];
      cout << "TOTAL (" << j << ") -> " << tot << '\n';
    }
    print_PMU();
    return 0;
}