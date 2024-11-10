#include <utility/ostream.h>
#include <real-time.h>
#include <architecture/cpu.h>
#include <scheduler.h>

using namespace EPOS;

OStream cout;

const unsigned int iterations = 10;
const Milisecond period_a = 100;
const Milisecond period_b = 80;
const Milisecond period_c = 167;
const Milisecond period_d = 90;
const Milisecond period_e = 210;
const Milisecond period_f = 270;
const Milisecond period_g = 344;
const Milisecond period_h = 190;
const Milisecond wcet_a = 50;
const Milisecond wcet_b = 20;
const Milisecond wcet_c = 60;
const Milisecond wcet_d = 40;
const Milisecond wcet_e = 30;
const Milisecond wcet_f = 25;
const Milisecond wcet_g = 45;
const Milisecond wcet_h = 35; 


int my_func(char id, int limit) {
    static int exec_count[8] = {0};
    do {
    long long b = 0;
    for (int i = 0; i < limit; ++i) {
       b += i - i/3;
    }
    exec_count[id - 'A']++;
    OStream cout;
    cout << "Thread " << id << " completed iteration. Total executions: " << exec_count[id - 'A'] << '\n';
    } while (Periodic_Thread::wait_next());
    
    return 0;
}
int my_func_aperiodic(char id) {
    long long b = 0;
    for (int i = 0; i < 1e1; ++i) {
       b += i - i/3;
    }
    OStream cout;
    cout << b << '\n';
    
    return 0;
}

int main()
{   //                                       p,               d, c,             act, t
    // thread_a = new Periodic_Thread(RTConf(period_a * 1000, 0, wcet_a * 1000,   0, iterations), &func_a);
    // thread_b = new Periodic_Thread(RTConf(period_b * 1000, 0, wcet_b * 1000,   0, iterations), &func_b);
    // thread_c = new Periodic_Thread(RTConf(period_c * 1000, 0, wcet_c * 1000,   0, iterations), &func_c);
    /*
    const unsigned int iterations = 100;
    const Milisecond period_a = 100;
    const Milisecond period_b = 80;
    const Milisecond period_c = 60;
    const Milisecond wcet_a = 50;
    const Milisecond wcet_b = 20;
    const Milisecond wcet_c = 10;
    */
    //                                              p              , d              , c            , a, n         , task_type
    Periodic_Thread* a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, wcet_a * 1000, 0, iterations, EDF_Modified::CRITICAL)   , &my_func, 'A', (int)1e6);
    Periodic_Thread* b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, wcet_b * 1000, 0, iterations, EDF_Modified::CRITICAL), &my_func, 'B', (int)1e7);
    Periodic_Thread* c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, wcet_c * 1000, 0, iterations, EDF_Modified::CRITICAL)   , &my_func, 'C', (int)2.5e7);
    Periodic_Thread* d = new Periodic_Thread(RTConf(period_b * 1000, period_d * 1000, wcet_d * 1000, 0, iterations, EDF_Modified::CRITICAL), &my_func, 'D', (int)1e7);
    Periodic_Thread* e = new Periodic_Thread(RTConf(period_c * 1000, period_e * 1000, wcet_e * 1000, 0, iterations, EDF_Modified::BEST_EFFORT), &my_func, 'E', (int)1e6);
    Periodic_Thread* f = new Periodic_Thread(RTConf(period_b * 1000, period_f * 1000, wcet_f * 1000, 0, iterations, EDF_Modified::BEST_EFFORT), &my_func, 'F', (int)1e7);
    Periodic_Thread* g = new Periodic_Thread(RTConf(period_c * 1000, period_g * 1000, wcet_g * 1000, 0, iterations, EDF_Modified::BEST_EFFORT), &my_func, 'G', (int)2.5e7);
    Periodic_Thread* h = new Periodic_Thread(RTConf(period_b * 1000, period_h * 1000, wcet_h * 1000, 0, iterations, EDF_Modified::BEST_EFFORT), &my_func, 'H', (int)1e7);
    
    
    cout << "A: " << a << " B: " << b << " C: " << c << " D: " << d << "E: "  << e << "F: " << f << "G: " << g << "H: " << h << endl;

    // cout << "B: " << b << " D: " << d << endl;

    a->join();
    b->join();
    c->join();
    d->join();
    
    return 0;
}