// EPOS Periodic Thread Abstraction Test Program

#include <utility/ostream.h>
#include <periodic_thread.h>
#include <chronometer.h>

__USING_SYS

const int iterations = 100;
const long period_a = 100; // ms
const long period_b = 100; // ms
const long period_c =  62; // ms
const long wcet_a = 40; // ms
const long wcet_b = 20; // ms
const long wcet_c = 12; // ms

int func_a(void);
int func_b(void);
int func_c(void);
void print(char c);
long max(long a, long b, long c) { return ((a >= b) && (a >= c)) ? a : ((b >= a) && (b >= c) ? b : c); }

OStream cout;

Chronometer chrono;

int main()
{
    cout << "Periodic Thread Abstraction Test\n";

    cout << "\nThis test consists in creating three periodic threads as follows:\n";
    cout << "- Every " << period_a << "ms, thread A prints \"a\", waits for " << wcet_a << "ms and then prints another \"a\";\n";
    cout << "- Every " << period_b << "ms, thread B prints \"b\", waits for " << wcet_b << "ms and then prints another \"b\";\n";
    cout << "- Every " << period_c << "ms, thread C prints \"c\", waits for " << wcet_c << "ms and then prints another \"c\";\n";

    cout << "Threads will now be created and I'll wait for them to finish...\n";

    Periodic_Thread thread_a(&func_a, period_a * 1000, iterations);
    print('A');
    Periodic_Thread thread_b(&func_b, period_b * 1000, iterations);
    print('B');
    Periodic_Thread thread_c(&func_c, period_c * 1000, iterations);
    print('C');

    print('M');

    chrono.start();

    int status_a = thread_a.join();
    print('A');
    int status_b = thread_b.join();
    print('B');
    int status_c = thread_c.join();
    print('C');

    chrono.stop();

    print('M');

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
    for(int i = 0; i < iterations; i++) {
        print('a');
        // Delay was not used here to prevent scheduling interference due to blocking
        for(Alarm::Microsecond t = Alarm::elapsed() + wcet_a * 1000; t > Alarm::elapsed(););
        print('a');
        Periodic_Thread::wait_next();
    }
    return 'A';   
}

int func_b()
{
    for(int i = 0; i < iterations; i++) {
        print('b');
        // Delay was not used here to prevent scheduling interference due to blocking
        for(Alarm::Microsecond t = Alarm::elapsed() + wcet_b * 1000; t > Alarm::elapsed(););
        print('b');
        Periodic_Thread::wait_next();
    }
    return 'B';   
}

int func_c()
{
    for(int i = 0; i < iterations; i++) {
        print('c');
        // Delay was not used here to prevent scheduling interference due to blocking
        for(Alarm::Microsecond t = Alarm::elapsed() + wcet_c * 1000; t > Alarm::elapsed(););
        print('c');
        Periodic_Thread::wait_next();
    }
    return 'C';
}

void print(char c)
{
    Alarm::Microsecond elapsed = Alarm::elapsed() / 1000;
    cout << "\n" << elapsed << ":\t" << c << " [pri=" << Thread::self()->priority() / 1000 << "]";
}
