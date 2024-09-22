#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <utility/string.h>
#include <utility/vector.h>
#include <utility/ostream.h>
#include "./thread.h"
#include "./scheduler.h"
#include "system/traits.h"

class Simulator {
    public:
        void start(float min_frequency, float max_frequency, ThreadArgs uncreated_thread_args[], int num_threads, Scheduler& scheduler);
    private:
        float _min_frequency;
        float _max_frequency;     
        const Thread* _current_thread;
        unsigned int _current_time{0};
        Scheduler _scheduler;   
        EPOS::OStream cout;
        
        void log(const char* text) {
            cout << '(' << _current_thread->id() << ") " << _current_thread << " --> " << text << '\n';
        }
};

#endif