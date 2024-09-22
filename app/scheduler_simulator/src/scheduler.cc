#include "../include/scheduler.h"

void Scheduler::create_thread(ThreadArgs& args) {
    auto *thread = new Thread(args);
    _threads.insert(&thread->element());
}

Thread* Scheduler::choose_next(unsigned int global_time) {
    return _threads.head()->object();
}

void Scheduler::finish_current_thread() {
    auto *thread = _threads.remove();
    delete thread;
}
