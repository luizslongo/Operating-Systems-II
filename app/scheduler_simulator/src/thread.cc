#include "../include/thread.h"
#include <utility/math.h>

int Thread::type() const {
    return _type;
}

unsigned int Thread::deadline() const {
    return _deadline;
}

unsigned int Thread::creation_time() const {
    return _creation_time;
}

EPOS::Ordered_Queue<Thread>::Element& Thread::element() {
    return _element;
}

unsigned int Thread::new_frequency(unsigned int current_time, float min_cpu_frequency, float max_cpu_frequency) const {
    int time_fraction = (_deadline-current_time)/(_deadline-_creation_time);
    float deadline_multiplier;
    
    if (time_fraction > 0.8) {
        deadline_multiplier = 1;
    } 
    else {
        deadline_multiplier = 1.35*(pow(2,time_fraction)-1);
    }

    int new_cpu_frequency = min_cpu_frequency + deadline_multiplier*(max_cpu_frequency-min_cpu_frequency);
    return new_cpu_frequency;
}

unsigned int Thread::execute(unsigned int current_time) const {
    return current_time++;
}

bool Thread::finished() const {
    return _task_time <= _execution_time;
}

int Thread::id() const {
    return _id;
}
