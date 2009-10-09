// EPOS-- Mutex Abstraction Test Program

#include <utility/ostream.h>
#include <thread.h>
#include <mutex.h>
#include <alarm.h>
#include <display.h>

__USING_SYS

const int iterations = 10;

Thread * phil[5];
Mutex chopstick[5];
Mutex display_mutex;

OStream cout;

int philosopher(int n, int l, int c)
{
//     cout << "I'm the philosopher " << n << "\n";
//     cout << "I'll print at " << l << " x " << c << "\n";
 
    int first = (n < 4)? n : 0;
    int second = (n < 4)? n + 1 : 4;

    for(int i = iterations; i > 0; i--) {
	display_mutex.lock();
	Display::position(l, c);
 	cout << "thinking";
	display_mutex.unlock();
	Delay thinking (1000000);

	chopstick[first].lock();   // get first chopstick
	chopstick[second].lock();   // get second chopstick
	display_mutex.lock();
	Display::position(l, c);
	cout << " eating ";
	display_mutex.unlock();
	Delay eating(500000);
	chopstick[first].unlock();   // release first chopstick
	chopstick[second].unlock();   // release second chopstick
    }

    display_mutex.lock();
    Display::position(l, c);
    cout << "  done  ";
    display_mutex.unlock();

    return(iterations);
}

int main()
{
    display_mutex.lock();
    Display::clear();
    cout << "The Philosopher's Dinner:\n";

    phil[0] = new Thread(&philosopher, 0, 5, 32);
    phil[1] = new Thread(&philosopher, 1, 10, 44);
    phil[2] = new Thread(&philosopher, 2, 16, 39);
    phil[3] = new Thread(&philosopher, 3, 16, 24);
    phil[4] = new Thread(&philosopher, 4, 10, 20);

    cout << "Philosophers are alive and hungry!\n";

    cout << "The dinner is served ...\n";
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
    display_mutex.unlock();


    for(int i = 0; i < 5; i++) {
	int ret = phil[i]->join();
	display_mutex.lock();
	Display::position(20 + i, 0);
	cout << "Philosopher " << i << " ate " << ret << " times \n";
	display_mutex.unlock();
    }

    for(int i = 0; i < 5; i++)
	delete phil[i];
    
    cout << "The end!\n";
    
    return 0;
}
