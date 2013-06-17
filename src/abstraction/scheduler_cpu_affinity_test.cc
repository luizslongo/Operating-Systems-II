// EPOS Scheduler Test Program

#include <utility/ostream.h>
#include <machine.h>
#include <display.h>
#include <thread.h>
#include <semaphore.h>
#include <alarm.h>
//#include <perf_mon.h>
#include <periodic_thread.h>
#include <utility/random.h>
#include <clock.h>
#include <chronometer.h>

__USING_SYS

#define ITERATIONS 200 // number of times each thread will be executed ~40seg
#define TEST_REPETITIONS 1
#define THREADS 1 // number of periodic threads
Periodic_Thread * threads[THREADS]; // periodics threads that will be created
Clock clock;
Semaphore sem;
Display d;

#define KB_256 1024*256
#define KB_512 1024*512
#define ARRAY_SIZE KB_256
#define MEMORY_ACCESS 16384
#define WRITE_RATIO 4
int job(unsigned int, int); // function passed by each periodic thread

RTC::Microsecond *wcet[THREADS];

// period (microsecond), deadline, execution time (microsecond), cpu (partitioned)
// 17 threads, total utilization of 5.972483 , 8 processors 
unsigned int lowest_priority_task = 16 ;
unsigned int threads_parameters[][4] = {
{ 50000 , 50000 , 32329 , 0 },
{ 50000 , 50000 , 5260 , 1 },
{ 50000 , 50000 , 12295 , 2 },
{ 200000 , 200000 , 62727 , 3 },
{ 100000 , 100000 , 49286 , 4 },
{ 200000 , 200000 , 48083 , 5 },
{ 200000 , 200000 , 22563 , 6 },
{ 100000 , 100000 , 17871 , 7 },
{ 25000 , 25000 , 15211 , 1 },
{ 200000 , 200000 , 129422 , 6 },
{ 200000 , 200000 , 52910 , 7 },
{ 100000 , 100000 , 14359 , 5 },
{ 25000 , 25000 , 14812 , 2 },
{ 50000 , 50000 , 33790 , 3 },
{ 25000 , 25000 , 7064 , 5 },
{ 100000 , 100000 , 20795 , 7 },
{ 200000 , 200000 , 42753 , 4 }
};

#define POLLUTE_BUFFER_SIZE KB_512
int pollute_cache(unsigned int repetitions, int id);
int run(int test);

RTC::Microsecond exec_time;
OStream cout;

int main()
{
  	d.clear();  

	cout << "P-EDF application!\n";
	  
  	exec_time = 0;
	  
  	for(int i = 0; i < THREADS; i++)
		wcet[i] = new unsigned long(sizeof(RTC::Microsecond) * ITERATIONS);
  
  	for(int i = 0; i <  TEST_REPETITIONS; i++) {
		cout << "Starting test " << i << "\n";
 		run(i);
  	}
	  
  	cout << "P-EDF application done!\n";
  	cout << "Execution time = " << exec_time / 1000000 << " seconds!\n";
	  
  	for(int i = 0; i < THREADS; i++)
		delete wcet[i];
	  

	while(1);
}

int run(int test)
{
    Chronometer chrono;
    
    //for(unsigned int i = 0; i < 0xffffffff; i++)
    //    for(unsigned int j = 0; j < 0xffffffff; j++) ;
        
    for(int i = 0; i <  THREADS; i++) 
        for(int j = 0; j < ITERATIONS; j++)
            wcet[i][j] = 0;

    for(int i = 0; i <  THREADS; i++) {
		if(i == lowest_priority_task) {
		    threads[i] = new Periodic_Thread(&pollute_cache,
		                                (unsigned int) (threads_parameters[i][2] / 1730) * 10,
		                                i, //ID
		                                threads_parameters[i][0], //period
										ITERATIONS, //number of iterations
										Thread::READY,
		                                Thread::Criterion((RTC::Microsecond) threads_parameters[i][0], (RTC::Microsecond) threads_parameters[i][0], threads_parameters[i][2], threads_parameters[i][3])
		                                ); 
		} else {
		   
		    threads[i] = new Periodic_Thread(&job, 
		                                (unsigned int) ((threads_parameters[i][2] / 540) * 3),
		                                i, //ID
		                                threads_parameters[i][0], //period
										ITERATIONS, //number of iterations
										Thread::READY,
		                                Thread::Criterion((RTC::Microsecond) threads_parameters[i][0], (RTC::Microsecond) threads_parameters[i][0], threads_parameters[i][2], threads_parameters[i][3])
		                                ); 
		}
  }
  
  chrono.start();
  
  for(int i = 0; i <  THREADS; i++) {
    threads[i]->join();
  }
  
  chrono.stop();
  
    
  for(int i = 0; i <  THREADS; i++) {
    delete threads[i];
  }
  
  if(chrono.read() > exec_time)
      exec_time = chrono.read();
  
  cout << "P-EDF test " << test << " done in " << chrono.read() / 1000000 << " seconds!\n";

}

int pollute_cache(unsigned int repetitions, int id)
{
    int sum = 0;
    Chronometer c;
    Pseudo_Random * rand;
    int *pollute_buffer;
    
    rand = new Pseudo_Random();
    
    rand->seed(clock.now() + id);
    
    pollute_buffer = new int[POLLUTE_BUFFER_SIZE];
    
    for(int i = 0; i <  ITERATIONS; i++) {
      Periodic_Thread::wait_next();
      
      c.start();
     
      for(int j = 0; j < repetitions; j++) {
        for(int k = (rand->random() % (POLLUTE_BUFFER_SIZE - 1) ) % 1000; k < POLLUTE_BUFFER_SIZE; k += 64) {
            pollute_buffer[k] = j % 64;
            sum += pollute_buffer[k];
        }
      }
      
      c.stop();
      
      wcet[id][i] = c.read();
      
      c.reset();
    }
    
    delete rand;
    delete pollute_buffer;
    
    return sum;
}

int job(unsigned int repetitions, int id)
{
    int sum = 0;
    Chronometer c;
    Pseudo_Random * rand;
    int *array;
	unsigned long long llc_misses = 0;
	unsigned long long llc_hit = 0;
	//Perf_Mon perf;

	sem.p();
	cout << "Starting thread = " << id << "\n";
	sem.v();

    rand = new Pseudo_Random();
        
    rand->seed(clock.now() + id);

    array = new int[ARRAY_SIZE];
    
    for(int i = 0; i <  ITERATIONS; i++) {
      Periodic_Thread::wait_next();
	  //perf.llc_misses();
	  //perf.llc_hit();
      c.reset();
      
      c.start();
      
      for(int j = 0; j < repetitions; j++) {
        for(int k = 0; k < MEMORY_ACCESS; k++) {
            int pos = rand->random() % (ARRAY_SIZE - 1);            
            sum += array[pos];
            if((k % WRITE_RATIO) == 0)
              array[pos] = k + j;
        }
      }
      
      c.stop();
      
      wcet[id][i] = c.read();

      c.reset();

	  //llc_misses = (llc_misses + perf.get_llc_misses()) / 2;
	  //llc_hit = (llc_hit + perf.get_llc_hit()) / 2;
      
    }
    
    delete rand;
    delete array;

	sem.p();
    //cout << "LLC Misses = " << llc_misses << " LLC Hits = " << llc_hit << "\n";
	cout << "Thread " << id << " has finished\n";
	sem.v();

    return sum;
}
