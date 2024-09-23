#include "../include/simulator.h"
#include "../include/thread.h"
#include <utility/ostream.h>
#include <utility/string.h>

void Simulator::start(float min_frequency, float max_frequency, ThreadArgs uncreated_thread_args[], int num_threads, Scheduler& scheduler) {
    _min_frequency = min_frequency;
    _max_frequency = max_frequency;
    _current_time = 0;
    Thread* next_thread;
    _current_thread = nullptr;

    int thread_idx = 0;
    
    // Main loop of the simulation, executes while there are still not finished threads, threads to be created or an executing thread
    for (int i = 0; i < 60; i++) {
    
    //while (!_scheduler.all_finished() || thread_idx < num_threads || _current_thread != nullptr) {

        // In case the CPU is idle and it is not yet the moment to create a new thread, simply increment the current time
        if (thread_idx < num_threads && _current_time < uncreated_thread_args[thread_idx].creation_time && _current_thread == nullptr) {
            _current_time++;
            continue;
        }

        // In case a new thread should be created, creates a new thread 
        // (the id of the last created thread is smaller than the total numbers of thread 
        // and the the simulator has reached the creation time of the next to-be-created thread)
        if (thread_idx < num_threads && _current_time >= uncreated_thread_args[thread_idx].creation_time) {
            log("Creating Thread");
            // Creates the thread and increments the threads index
            scheduler.create_thread(uncreated_thread_args[thread_idx++]);
        }

        // The scheduler chooses the next thread to execute, based on the current time
        next_thread = scheduler.choose_next();

        if (next_thread == nullptr) {
            _current_thread = nullptr;
            log("No thread running, idling...");
            continue;
        }
        
        // On EPOS, the new frequency will be calculated if the flag "charge" is true
        int frequency = next_thread->new_frequency(_current_time, _min_frequency, _max_frequency);
        
        cout << '(' << _current_thread->id() << ") " << _current_time << " --> " << "Setting new frequency to " << frequency << '\n';
        
        if (next_thread != _current_thread)
            _current_thread = next_thread; // Simulating context switch.
        
        // Here we assume the thread only executes for a fixed period of time.
        // This is needed to make sure our algorithm works correctly.
        // Inside EPOS, this will be done by setting the timed attribute to `true`.
        //
        // Right now, you can assume this thread is executed for 1 unit of time.
        log("Executing Thread for 1 unit of time...");
        _current_thread->execute(_current_time);
        _current_time++;

        if (_current_thread->finished()) {
            _scheduler.finish_current_thread();
            log("Thread finished execution.");
            _current_thread = nullptr;
        }
    }
}
