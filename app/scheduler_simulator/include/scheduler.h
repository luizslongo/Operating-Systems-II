#ifndef SCHEDULER_H
#define SCHEDULER_H

//#include <vector>
// #include <queue>
#include <utility/queue.h>
#include <utility/math.h>
#include "./thread.h"


class Scheduler{
    public:
        Thread* choose_next(unsigned int global_time);
        void create_thread(ThreadArgs& arg);
        void finish_current_thread();

    private:
        class CompareByDeadline {
            friend bool operator<(Thread& a, Thread& b) {
                if (a.type() == Thread::CRITICAL && b.type() != Thread::CRITICAL)
                    return false;
                else if (a.type() != Thread::CRITICAL && b.type() == Thread::CRITICAL)
                    return true;
                return a.deadline() > b.deadline();

            }
        };
        EPOS::Ordered_Queue<Thread> _threads;    
};

#endif