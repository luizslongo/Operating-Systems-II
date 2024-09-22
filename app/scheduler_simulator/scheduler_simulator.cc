#include <utility/string.h>
#include <utility/ostream.h>
#include "./include/thread.h"
#include "./include/simulator.h"
#include "./include/scheduler.h"

//OStream cout;
int main(int argc, char *argv[])
{
    double min_cpu_frequency = 0.25;
    double max_cpu_frequency = 1;
    // struct ThreadArgs {
    //     int id;
    //     int type;
    //     unsigned int deadline;
    //     unsigned int creation_time;
    //     unsigned int task_time;
    // };
    ThreadArgs t1{0, 1, 15, 0, 8};
    ThreadArgs t2{1, 0, 12, 3, 5};
    ThreadArgs t3{2, 0, 14, 8, 4};

    //std::priority_queue<int> q;

    
    ThreadArgs arr[3] = {t1, t2, t3};
    
    Simulator sim;
    Scheduler sched;
    sim.start(min_cpu_frequency, max_cpu_frequency, arr, 3, sched);

    return 0;
}
