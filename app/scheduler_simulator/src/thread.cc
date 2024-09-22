#include "../include/thread.h"
#include <utility/math.h>

int Thread::type() {
    return _type;
}

unsigned int Thread::deadline() {
    return _deadline;
}

unsigned int Thread::creation_time() {
    return _creation_time;
}

EPOS::Ordered_Queue<Thread>::Element& Thread::element() {
    return _element;
}

double Thread::new_frequency(unsigned int current_time, double min_cpu_frequency, double max_cpu_frequency) {
    double time_fraction = 1.0 - static_cast<double>((_deadline-current_time))/static_cast<double>((_deadline-_creation_time));
    double deadline_multiplier;
    
    if (time_fraction > 0.8) {
        deadline_multiplier = 1.0;
    } 
    else {
        deadline_multiplier = 1.35*(pow(2,time_fraction)-1.0);
    }
    EPOS::OStream cout;
    cout << "deadline " << _deadline << "\n";
    cout << "current_time " << current_time << "\n";
    cout << "creation_time " << _creation_time << "\n";
    cout << "time_fraction " << time_fraction << "\n";
    cout << "deadline_multiplier " << deadline_multiplier << "\n";
    cout << "execution_time" << _execution_time << "\n";
    cout << "min_cpu " << min_cpu_frequency << "\n";
    cout << "max_cpu " << max_cpu_frequency << "\n";
    
    double new_cpu_frequency = min_cpu_frequency + deadline_multiplier*static_cast<double>((max_cpu_frequency-min_cpu_frequency));
    cout << "new_cpu " << new_cpu_frequency << "\n";
    return new_cpu_frequency;
}

unsigned int Thread::execute(unsigned int current_time) {
    _execution_time++;
    return current_time++;
}

bool Thread::finished() {
    return _task_time <= _execution_time;
}

int Thread::id() {
    return _id;
}
