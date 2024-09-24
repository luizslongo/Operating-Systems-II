#ifndef SCHEDULER_H
#define SCHEDULER_H

// #include <vector>
//  #include <queue>
#include "./thread.h"
#include <utility/math.h>
#include <utility/queue.h>

// Thread-managing responsible Class
class Scheduler {
public:
  // Chooses the next thread to be executed, based on the global time
  Thread *choose_next(unsigned int global_time);
  // Creates a new thread
  void create_thread(ThreadArgs &arg);
  // Finishes the currently executing thread
  void finish_current_thread(Thread *current);
  // Verify if all of the threads have been finished
  bool all_finished();

private:
  // Internal class used to compare threads based in their deadline
  class CompareByDeadline {
    // Overload of the '<' operator to campre two threads
    // Critical threads have higher priority; otherwise compares using deadlines
    friend bool operator<(Thread &a, Thread &b) {
      // If the current thread is CRITICAL, and the other is not, then it has
      // less priority
      if (a.type() == Thread::CRITICAL && b.type() != Thread::CRITICAL)
        return false;
      // In case both thread are the same type, compares them by deadline
      // (shortest deadline has priority)
      else if (a.type() != Thread::CRITICAL && b.type() == Thread::CRITICAL)
        return true;
      return a.deadline() > b.deadline();
    }
  };
  // Ordered queue of threads, that organizaes them based on the criteria
  // defined on the class "CompareByDeadline"
  EPOS::Ordered_Queue<Thread> _threads;
};

#endif