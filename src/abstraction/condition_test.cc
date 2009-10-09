// EPOS-- Semaphore Test Program

#include <utility/ostream.h>
#include <display.h>
#include <thread.h>
#include <alarm.h>
#include <mutex.h>
#include <condition.h>

__USING_SYS

const int iterations = 10;

Thread * phil[5];
Mutex chopstick[5];
Condition barrier;

OStream cout;

int philosopher(int n, int l, int c)
{
    
    cout << "I'm the philosopher " << n << "\n";
    cout << "I'll print at " << l << " x " << c << "\n";
 
    int first = (n < 4)? n : 0;
    int second = (n < 4)? n + 1 : 4;

    barrier.wait();

    for(int i = iterations; i > 0; i--) {
	Display::position(l, c);
 	cout << "thinking";
	Delay thinking(1000000);

	chopstick[first].lock();   // get first chopstick
	chopstick[second].lock();   // get second chopstick
	Display::position(l, c);
	cout << " eating ";
	Delay eating(500000);
	chopstick[first].unlock();   // release first chopstick
	chopstick[second].unlock();   // release second chopstick
    }

    return(iterations);
}

int main()
{
    Display display;

    Display::clear();
    cout << "The Philosopher's Dinner:\n";

    phil[0] = new Thread(&philosopher, 0, 5, 32);
    phil[1] = new Thread(&philosopher, 1, 10, 44);
    phil[2] = new Thread(&philosopher, 2, 16, 39);
    phil[3] = new Thread(&philosopher, 3, 16, 24);
    phil[4] = new Thread(&philosopher, 4, 10, 20);

    cout << "Philosophers are alife and hungry!\n";

    Alarm::delay(2000000);
    barrier.broadcast();

    cout << "The dinner is served!\n";
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

    Alarm::delay(6000000);

    for(int i = 0; i < 5; i++) {
	int ret = phil[i]->join();
	Display::position(20 + i, 0);
	cout << "Philosopher " << i << " ate " << ret << " times \n";
    }

    for(int i = 0; i < 5; i++)
	delete phil[i];
    
    cout << "The end!\n";

    return 0;
}
