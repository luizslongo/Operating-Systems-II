#include "../include/scheduler.h"
#include "system/config.h"

void Scheduler::create_thread(ThreadArgs &args) {
  EPOS::OStream cout;
  auto *thread = new Thread(args);
  _threads.insert(&thread->element());
}

Thread *Scheduler::choose_next(unsigned int global_time) {
  if (_threads.empty())
    return nullptr;

  auto *chosen_thread = _threads.head();
  while (chosen_thread != nullptr &&
         chosen_thread->object()->creation_time() > global_time)
    chosen_thread = chosen_thread->next();

  return chosen_thread ? chosen_thread->object() : nullptr;
}

void Scheduler::finish_current_thread(Thread *current) {
  if (all_finished())
    return;
  EPOS::OStream cout;
  auto *element = _threads.remove(&current->element());
  element->object()->recalculate_times();

  cout << "Readding thread\n";
  cout << "New Deadline: " << element->object()->deadline() << "\n";
  cout << "New Creation Time: " << element->object()->creation_time() << "\n";
  _threads.insert(element);
}

bool Scheduler::all_finished() { return _threads.empty(); }