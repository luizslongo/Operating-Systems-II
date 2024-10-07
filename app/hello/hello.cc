#include <utility/ostream.h>
#include <time.h>
#include <real-time.h>
#include <architecture/cpu.h>
#include <scheduler.h>

using namespace EPOS;

OStream cout;

const unsigned int iterations = 10;
const Milisecond period_a = 100;
const Milisecond period_b = 80;
const Milisecond period_c = 200;
const Milisecond wcet_a = 50;
const Milisecond wcet_b = 20;
const Milisecond wcet_c = 60;


int my_func(char id) {
    do {
    long long b = 0;
    for (int i = 0; i < 2.5e7; ++i) {
       b += i - i/3;
    }
    OStream cout;
    cout << b << '\n';
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
    Periodic_Thread* a = new Periodic_Thread(RTConf(period_a * 1000, period_a * 1000, wcet_a * 1000, 0, iterations, EDF_Modified::CRITICAL)   , &my_func, 'A');
    Periodic_Thread* b = new Periodic_Thread(RTConf(period_b * 1000, period_b * 1000, wcet_b * 1000, 0, iterations, EDF_Modified::BEST_EFFORT), &my_func, 'B');
    Periodic_Thread* c = new Periodic_Thread(RTConf(period_c * 1000, period_c * 1000, wcet_a * 1000, 0, iterations, EDF_Modified::CRITICAL)   , &my_func, 'C');
    Thread* d = new Thread(&my_func_aperiodic, 'D');
    
    cout << "A: " << a << ", B: " << b << ", C: " << c << ", D: " << d << '\n';
    cout << "A: " << a->priority() << ", B: " << b->priority() << ", C: " << c->priority() << ", D: " << d->priority() << '\n';

    a->join();
    b->join();
    c->join();
    d->join();
    
    return 0;
}
