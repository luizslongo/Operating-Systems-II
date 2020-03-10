#include <utility/ostream.h>
#include <synchronizer.h>
#include <process.h>
#include <time.h>
#include <architecture/cpu.h>

using namespace EPOS;

OStream cout;

const bool SILENT = false;
const unsigned int THREADS = 4;
const unsigned int SLEEP_TIME = 500000; // 0.5 sec
const unsigned int ITERATIONS = 10; // 5 sec

Semaphore print;

Thread * thread[THREADS];

int test() {
    int cpu = CPU::id();
    if (!SILENT) {
        print.p();
        cout << "CPU[" << cpu << "] Start!" << endl;
        print.v();
    }

    for (unsigned int i = 0; i < ITERATIONS; ++i) {
        Delay(500000);
        if (!SILENT) {
            print.p();
            cout << "CPU[" << cpu << "] Iter["<< i <<"]" << ",&I=" << &i << endl;
            print.v();
        }
    }

    print.p();
    cout << "CPU[" << cpu << "] End!" << endl;
    print.v();

    return 0;
}

int main()
{
    cout << "Simple SMP Tester!" << endl;
    if (!SILENT) {
        cout << THREADS << " Threads will be created following CPU Affinity Scheduling!" << endl;
        cout << "Will create them now!" << endl;
    }
    
    print = Semaphore(1);
    print.p();

    for (unsigned int i = 0; i < THREADS; ++i) {
        thread[i] = new Thread(&test);
    }

    if (!SILENT) {
        cout << "All Threads have been created! \n Now they will sleep for 0.5s each and print a message for " << ITERATIONS << " Times \n Releasing print lock!" << endl;
    }
    print.v();

    for (unsigned int i = 0; i < THREADS; ++i) {
        thread[i]->join();
    }

    if (!SILENT) {
        cout << "All threads have ended!" << endl;
        cout << "Simple SMP Tester will now delete them!" << endl;
    }

    for (unsigned int i = 0; i < THREADS; ++i) {
        delete thread[i];
    }

    if (!SILENT) {
        cout << "Sleeping for 2 second, then the system will be rebooted!" << endl;
    }
    cout << "Goodbey world!" << endl;

    return 0;
}
