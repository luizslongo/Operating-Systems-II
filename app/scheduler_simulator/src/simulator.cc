#include "../include/simulator.h"
#include "../include/thread.h"
#include <utility/ostream.h>
#include <utility/string.h>

void Simulator::start(float min_frequency, float max_frequency,
                      ThreadArgs uncreated_thread_args[], int num_threads,
                      Scheduler &scheduler) {
  _min_frequency = min_frequency;
  _max_frequency = max_frequency;
  _current_time = 0;
  Thread *next_thread;
  _current_thread = nullptr;

  int thread_idx = 0;
  int average_frequency = 0;

  // Main loop of the simulation, executes while there are still not finished
  // threads, threads to be created or an executing thread
  for (int i = 0; i < 60; i++) {

    // while (!_scheduler.all_finished() || thread_idx < num_threads ||
    // _current_thread != nullptr) {

    // In case the CPU is idle and it is not yet the moment to create a new
    // thread, simply increment the current time
    if (thread_idx < num_threads &&
        _current_time < uncreated_thread_args[thread_idx].creation_time &&
        _current_thread == nullptr) {
      _current_time++;
      continue;
    }

    // In case a new thread should be created, creates a new thread
    // (the id of the last created thread is smaller than the total numbers of
    // thread and the the simulator has reached the creation time of the next
    // to-be-created thread)
    if (thread_idx < num_threads &&
        _current_time >= uncreated_thread_args[thread_idx].creation_time) {
      log("Creating Thread");
      // Creates the thread and increments the threads index
      scheduler.create_thread(uncreated_thread_args[thread_idx++]);
    }

    // The scheduler chooses the next thread to execute
    next_thread = scheduler.choose_next(_current_time);

    // If there is no thread to be executed, the simulator enters the idle mode
    if (next_thread == nullptr) {
      _current_thread = nullptr;
      log("No thread running, idling...");
      _current_time++;
      continue;
    }
    // In case the next chosen thread is different than the current thread,
    // simulates a context switch
    if (next_thread != _current_thread)
      _current_thread = next_thread; // Simulating context switch.

    // On EPOS, the new frequency will be calculated if the flag "charge" is
    // true
    int frequency = next_thread->new_frequency(_current_time, _min_frequency,
                                               _max_frequency);
    average_frequency += frequency;
    cout << "(Thread " << (_current_thread ? _current_thread->id() : -1) << ") "
         << _current_time << " --> " << "Setting new frequency to " << frequency
         << '\n';

    // Here we assume the thread only executes for a fixed period of time
    // This is needed to make sure our algorithm works correctly
    // Inside EPOS, this will be done by setting the timed attribute to 'true'
    //
    // Right now, you can assume this thread will be executed for 1 unit of
    // time.
    log("Executing Thread for 1 unit of time...");
    _current_thread->execute(_current_time, frequency);

    // If the current thread has finished its execution, remove it from the
    // scheduler
    if (_current_thread->finished()) {
      scheduler.finish_current_thread(_current_thread);
      log("Thread finished execution.");
      _current_thread = nullptr;
    }
    _current_time++;

    cout << "====================================== " << _current_time << " "
         << "======================================\n";
  }

  cout << "AVERAGE FREQUENCY LEVEL: " << average_frequency/60.0 << '\n';
}
