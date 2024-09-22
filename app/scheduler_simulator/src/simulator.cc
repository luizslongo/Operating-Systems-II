#include "../include/simulator.h"
#include "../include/thread.h"
#include <utility/ostream.h>
#include <utility/string.h>

void Simulator::start(float min_frequency, float max_frequency, ThreadArgs uncreated_thread_args[], int num_threads, Scheduler& scheduler) {
    _current_time = 0;
    const Thread* next_thread;
    _current_thread = nullptr;

    int thread_idx = 0;
    while (thread_idx < num_threads) {
        // New thread created
        if (thread_idx < num_threads && _current_time <= uncreated_thread_args[thread_idx].creation_time) {
            log("Creating Thread");
            scheduler.create_thread(uncreated_thread_args[thread_idx++]);
        }

        // Do scheduling operation.
        next_thread = scheduler.choose_next(_current_time);
        
        // On EPOS, the new frequency will be calculated if the flag "charge" is true
        double frequency = next_thread->new_frequency(_current_time, _min_frequency, _max_frequency);
        
        cout << '(' << _current_thread->id() << ") " << _current_thread << " --> " << "Setting new frequency to " << frequency << '\n';
        
        if (next_thread != _current_thread)
            _current_thread = next_thread; // Simulating context switch.
        
        // Here we assume the thread only executes for a fixed period of time.
        // This is needed to make sure our algorithm works correctly.
        // Inside EPOS, this will be done by setting the timed attribute to `true`.
        //
        // Right now, you can assume this thread is executed for 1 unit of time.
        log("Executing Thread for 1 unit of time...");
        _current_time = _current_thread->execute(_current_time);

        if (_current_thread->finished()) {
            _scheduler.finish_current_thread();
            log("Thread finished execution.");
        }
    }
}
