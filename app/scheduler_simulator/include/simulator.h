#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "./scheduler.h"
#include "./thread.h"
#include "system/traits.h"
#include <utility/ostream.h>
#include <utility/string.h>
#include <utility/vector.h>

// This class simulates the creation, scheduling and execution of threads
class Simulator {
public:
  // Initializes the simulation with minimum and maximum CPU frequencies, thread
  // arguments and the scheduler
  void start(float min_frequency, float max_frequency,
             ThreadArgs uncreated_thread_args[], int num_threads,
             Scheduler &scheduler);

private:
  // These are used to adjust the performance of the threads (in percentage)
  float _min_frequency;
  float _max_frequency;
  // Pointer to the thread currently executing
  Thread *_current_thread;
  // Current time in the simulator, it is incremented in each simulation step
  unsigned int _current_time{0};
  // Object to register outputs and logs of the simulation
  EPOS::OStream cout;

  // Internal function that registers the simulation logs, associating the
  // thread ID and the current time to the message
  void log(const char *text) {
    cout << "(Thread "
         << (_current_thread != nullptr ? _current_thread->id() : -1) << ") "
         << _current_time << " --> " << text << '\n';
  }
};

#endif