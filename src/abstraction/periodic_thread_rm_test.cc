// EPOS Periodic Thread Abstraction Test Program

#include <utility/ostream.h>
#include <periodic_thread.h>
#include <chronometer.h>

__USING_SYS

const unsigned int iterations = 100;
const unsigned int period_a = 100; // ms
const unsigned int period_b = 80; // ms
const unsigned int period_c = 60; // ms
const unsigned int wcet_a = 50; // ms
const unsigned int wcet_b = 20; // ms
const unsigned int wcet_c = 10; // ms

int func_a();
int func_b();
int func_c();
long max(unsigned int a, unsigned int b, unsigned int c) { return ((a >= b) && (a >= c)) ? a : ((b >= a) && (b >= c) ? b : c); }

OStream cout;
Chronometer chrono;
Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;

inline void exec(char c, Alarm::Microsecond time = 0)
{
    Alarm::Microsecond elapsed = Alarm::elapsed() / 1000;
    cout << "\n" << elapsed << "\t" << c
         << "\t[p(A)=" << thread_a->priority() / 1000
         << ", p(B)=" << thread_b->priority() / 1000
         << ", p(C)=" << thread_c->priority() / 1000 << "]";
    if(time) {
        // Delay was not used here to prevent scheduling interference due to blocking
        for(Alarm::Microsecond end = elapsed + time, last = end; end > elapsed; elapsed = Alarm::elapsed() / 1000)
            if(last != elapsed) {
                cout << "\n" << elapsed << "\t" << c
                    << "\t[p(A)=" << thread_a->priority() / 1000
                    << ", p(B)=" << thread_b->priority() / 1000
                    << ", p(C)=" << thread_c->priority() / 1000 << "]";
                last = elapsed;
            }
    }
}


int main()
{
    cout << "Periodic Thread Abstraction Test\n";

    cout << "\nThis test consists in creating three periodic threads as follows:\n";
    cout << "- Every " << period_a << "ms, thread A execs \"a\", waits for " << wcet_a << "ms and then execs another \"a\";\n";
    cout << "- Every " << period_b << "ms, thread B execs \"b\", waits for " << wcet_b << "ms and then execs another \"b\";\n";
    cout << "- Every " << period_c << "ms, thread C execs \"c\", waits for " << wcet_c << "ms and then execs another \"c\";\n";

    cout << "Threads will now be created and I'll wait for them to finish...\n";

    thread_a = new Periodic_Thread(&func_a, period_a * 1000, iterations);
    thread_b = new Periodic_Thread(&func_b, period_b * 1000, iterations);
    thread_c = new Periodic_Thread(&func_c, period_c * 1000, iterations);

    exec('M');

    chrono.start();

    int status_a = thread_a->join();
    int status_b = thread_b->join();
    int status_c = thread_c->join();

    chrono.stop();

    exec('M');

    cout << "\n... done!\n";
    cout << "\n\nThread A exited with status \"" << char(status_a)
         << "\", thread B exited with status \"" << char(status_b)
         << "\" and thread C exited with status \"" << char(status_c) << ".\n";

    cout << "\nThe estimated time to run the test was "
         << max(period_a, period_b, period_c) * iterations
         << " ms. The measured time was " << chrono.read() / 1000 <<" ms!\n";

    cout << "I'm also done, bye!\n";

    return 0;
}

int func_a()
{
    exec('A');
    for(int i = 0; i < iterations; i++) {
        exec('a', wcet_a);
        Periodic_Thread::wait_next();
    }
    exec('A');
    return 'A';   
}

int func_b()
{
    exec('B');
    for(int i = 0; i < iterations; i++) {
        exec('b', wcet_b);
        Periodic_Thread::wait_next();
    }
    exec('B');
    return 'B';   
}

int func_c()
{
    exec('C');
    for(int i = 0; i < iterations; i++) {
        exec('c', wcet_c);
        Periodic_Thread::wait_next();
    }
    exec('C');
    return 'C';
}
