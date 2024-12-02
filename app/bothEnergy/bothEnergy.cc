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

const unsigned int iterations = 100;
const Milisecond period_a = 100;
const Milisecond period_c = 200;
const Milisecond period_e = 100;
const Milisecond period_g = 200;
const Milisecond wcet_a = 50;
const Milisecond wcet_c = 20;
const Milisecond wcet_e = 60;
const Milisecond wcet_g = 40;

void print_PMU() {
  cout << '<' << CPU::id() << "> L1 HITS                    : " << PMU::read(3)
       << '\n';
  cout << '<' << CPU::id() << "> L1 MISSES                  : " << PMU::read(4)
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
       << "%"
       << '\n';

  cout << '<' << CPU::id() << "> FINISHING MAIN               " << '\n';
  cout << "Thread ID (CPU ID) => Number of iterations\n";
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 4; ++j)
      cout << (char)('A' + i) << "(" << j <<  ") => " << exec_count[i][j] << '\n';
  for (int j = 0; j < 4; ++j) {
    int tot = 0;
    for (int i = 0; i < 8; ++i)
      tot += exec_count[i][j];
    cout << "TOTAL ITERATIONS PER CPU (" << j << ") -> " << tot << '\n';
  }
  print_PMU();
  return 0;
}