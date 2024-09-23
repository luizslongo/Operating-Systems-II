#include "../include/thread.h"
#include <utility/math.h>

// Returns the thread type (CRITICAL or BEST_EFFORT)
int Thread::type() {
    return _type;
}

// Returns the thread deadline
unsigned int Thread::deadline() {
    return _deadline;
}

// Returns the thread creation time
unsigned int Thread::creation_time() {
    return _creation_time;
}

// Returns the reference to the thread element in the ordered queue (used by the scheduler)
EPOS::Ordered_Queue<Thread>::Element& Thread::element() {
    return _element;
}

// Function resposible for calculating the new CPU frequency needed for the thread to meet its deadline
float Thread::new_frequency(unsigned int current_time, float min_cpu_frequency, float max_cpu_frequency) {
    // Protect against underflow in case the current time has already surpassed the deadline.
    if (current_time > _deadline)
        return max_cpu_frequency;
    
    // Calculates the fraction of time that has passed between the thread creation and its deadline (the total amount of time the thread has to execute)
    float time_fraction = 1.0 - static_cast<float>((_deadline-current_time))/static_cast<float>((_deadline-_creation_time));
    float deadline_multiplier;
    
    // In case more than 80% of the time has passed, the multiplier will be 1.0 (CPU will operate in maximum frequency)
    if (time_fraction > 0.8) {
        deadline_multiplier = 1.0;
    } 
    // Otherwise, applies a non-linear multiplier to accelerate the thread execution based on the time fraction 
    // (the CPU frequency will increase the more time passes, untill reaching a maximum frequency) 
    else {
        deadline_multiplier = 1.35*(pow2(time_fraction)-1.0);
    }
    // For debugging purposes
    EPOS::OStream cout;
    cout << "deadline " << _deadline << "\n";
    cout << "current_time " << current_time << "\n";
    cout << "creation_time " << _creation_time << "\n";
    cout << "time_fraction " << time_fraction << "\n";
    cout << "deadline_multiplier " << deadline_multiplier << "\n";
    cout << "execution_time " << _execution_time << "\n";
    cout << "min_cpu " << min_cpu_frequency << "\n";
    cout << "max_cpu " << max_cpu_frequency << "\n";
    
    // Calculats the new CPU frequency based on the multiplier in a way that it will never be above maximum and below minimum frequencies
    float new_cpu_frequency = min_cpu_frequency + deadline_multiplier*static_cast<float>((max_cpu_frequency-min_cpu_frequency));
    cout << "new_cpu " << new_cpu_frequency << "\n";

    new_cpu_frequency *= 10;
    cout << "new_cpu_level " << round(new_cpu_frequency) << "\n";
    return round(new_cpu_frequency);
}

// Function responsible for running the thread (simulate its execution for one unit of time)
unsigned int Thread::execute(unsigned int current_time) {
    _execution_time++;
    return current_time++;
}

void Thread::recalculate_times() {
    if (finished()) {
        _deadline = _deadline + _period_time;
        _execution_time = 0;
        _creation_time = _deadline;
    }
}

// Verifies if the thread has already finished its execution (its execution time will be greater than the total task time)
bool Thread::finished() {
    return _task_time <= _execution_time;
}

// Returns the unique thread ID
int Thread::id() {
    return _id;
}
