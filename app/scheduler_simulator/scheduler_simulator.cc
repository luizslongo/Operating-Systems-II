#include "./include/scheduler.h"
#include "./include/simulator.h"
#include "./include/thread.h"
#include <utility/ostream.h>
#include <utility/string.h>

// OStream cout;
int main(int argc, char *argv[]) {
  float min_cpu_frequency = 0.25;
  float max_cpu_frequency = 1;
  // struct ThreadArgs {
  //     int id;
  //     int type;
  //     unsigned int deadline;
  //     unsigned int creation_time;
  //     unsigned int task_time;
  // };
  ThreadArgs t1{0, 0, 20, 0, 3};
  ThreadArgs t2{1, 1, 10, 0, 2};
  ThreadArgs t3{2, 0, 25, 0, 6};
  ThreadArgs t4{3, 0, 30, 0, 5};

  ThreadArgs arr[4] = {t1, t2, t3, t4};

  Simulator sim;
  Scheduler sched;
  sim.start(min_cpu_frequency, max_cpu_frequency, arr, 4, sched);

  return 0;
}
