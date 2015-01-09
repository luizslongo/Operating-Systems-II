// EPOS Scheduler Test Program

#include <utility/ostream.h>
#include <machine.h>
#include <display.h>
#include <thread.h>
#include <semaphore.h>
#include <alarm.h>

using namespace EPOS;

const int iterations = 10;

Semaphore sem_display;

Thread * phil[5];
Semaphore * chopstick[5];

OStream cout;

int philosopher(int n, int l, int c)
{

    int first = (n < 4)? n : 0;
    int second = (n < 4)? n + 1 : 4;

    for(int i = iterations; i > 0; i--) {

        sem_display.p();
        Display::position(l, c);
 	cout << "thinking[" << Machine::cpu_id() << "]";
        sem_display.v();

        Delay thinking(100000);

        chopstick[first]->p();   // get first chopstick
        chopstick[second]->p();   // get second chopstick

        sem_display.p();
        Display::position(l, c);
        cout << " eating[" << Machine::cpu_id() << "] ";
        sem_display.v();

        Delay eating(500000);

        chopstick[first]->v();   // release first chopstick
        chopstick[second]->v();   // release second chopstick
    }

    sem_display.p();
    Display::position(l, c);
    cout << "  done[" << Machine::cpu_id() << "]  ";
    sem_display.v();

    return(iterations);
}

int main()
{
    sem_display.p();
    Display::clear();
    cout << "The Philosopher's Dinner:" << endl;

    for(int i = 0; i < 5; i++)
        chopstick[i] = new Semaphore;
        
    phil[0] = new Thread(&philosopher, 0,  5, 30);
    phil[1] = new Thread(&philosopher, 1, 10, 44);
    phil[2] = new Thread(&philosopher, 2, 16, 39);
    phil[3] = new Thread(&philosopher, 3, 16, 21);
    phil[4] = new Thread(&philosopher, 4, 10, 17);

    cout << "Philosophers are alive and hungry!" << endl;
        
    cout << "The dinner is served ..." << endl;
    Display::position(7, 44);
    cout << '/';
    Display::position(13, 44);
    cout << '\\';
    Display::position(16, 35);
    cout << '|';
    Display::position(13, 27);
    cout << '/';
    Display::position(7, 27);
    cout << '\\';
    sem_display.v();


    for(int i = 0; i < 5; i++) {
        int ret = phil[i]->join();
        sem_display.p();
        Display::position(20 + i, 0);
        cout << "Philosopher " << i << " ate " << ret << " times " << endl;
        sem_display.v();
    }

    for(int i = 0; i < 5; i++)
        delete chopstick[i];
    for(int i = 0; i < 5; i++)
        delete phil[i];

    cout << "The end!" << endl;

    return 0;
}
