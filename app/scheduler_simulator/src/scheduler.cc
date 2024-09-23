#include "../include/scheduler.h"
#include "system/config.h"

void Scheduler::create_thread(ThreadArgs& args) {
    EPOS::OStream cout;
    cout << "Size Queue before create new thread: " << _threads.size() << "\n";
    auto *thread = new Thread(args);
    _threads.insert(&thread->element());
    cout << "Size Queue after create new thread: " << _threads.size() << "\n";
}

Thread* Scheduler::choose_next() {
    if (_threads.empty())
        return nullptr;
    return _threads.head()->object();
}

void Scheduler::finish_current_thread() {
    // if (all_finished())
    //     return;
    //Element * remove(Element * e) { return T::remove(e); }
    EPOS::OStream cout;
    cout << "Size Queue Before Finish " << _threads.size() << "\n";
     auto *element = _threads.remove();
    _threads.head()->object()->recalculate_times();
    _threads.insert(element);
    cout << "Size Queue After Finish " << _threads.size() << "\n";
    //delete element;
}

bool Scheduler::all_finished() {
    return _threads.empty();
}