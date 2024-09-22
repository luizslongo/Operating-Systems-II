#ifndef THREAD_H
#define THREAD_H

#include "system/config.h"
#include<utility/queue.h>
#include<utility/math.h>

// Structure to store the arguments used to create a thread
/**
  * deadline
  * creation time
  * task type --> CRITICAL, BEST_EFFORT
  * task time --> time needed to execute 
  */

struct ThreadArgs {
    int id;
    int type;
    unsigned int deadline;
    unsigned int creation_time;
    unsigned int task_time;
};

// This represents a thread in the system
class Thread {
    public:
        // Enumeration of the thread types: CRITICAL (absolutely can NOT lose its deadline) 
        // and BEST_EFFORT (losing its deadline will not cause major issues)
        enum type : int {
            CRITICAL   = 0,
            BEST_EFFORT = 1
        };
        // Thread constructor
        Thread(ThreadArgs& args) :_id{args.id}, 
                                  _type(args.type), 
                                  _execution_time(0), 
                                  _creation_time(args.creation_time),
                                  _deadline(args.deadline), 
                                  // Element of the ordered queue, used by the scheduler
                                  _element(this, args.deadline | ( args.type != CRITICAL ?  1 << (sizeof(unsigned int) - 1) : 0 )),
                                  _task_time(args.task_time) {};
        // Public methods to get informations and execute the thread
        int type();
        int id();
        EPOS::Ordered_Queue<Thread>::Element& element();
        unsigned int deadline();
        unsigned int creation_time();
        bool finished(); //task_time >= execution_time

        // Calculates the new CPU frequency based on the current time, and the minimum and maximum allowed frequencies
        float new_frequency(unsigned int current_time, float min_cpu_frequency, float max_cpu_frequency);
        // Executes the thread for one unit of time and returns the new time
        unsigned int execute(unsigned int current_time);
    
    private:
        int _id;
        int _type;
        unsigned int _execution_time;
        unsigned int _creation_time;
        unsigned int _deadline;
        EPOS::Ordered_Queue<Thread>::Element _element;
        unsigned int _task_time;
        
        // The following methods exist to deal with EPOS limitations (exponentiation in which the exponent is a float)
        // This absolute value calculation is used in the method below
        float abs(float x) const {
            return x < 0 ? -x : x;
        }

        // Calculates the power of 2 using the bisection method
        float pow2(float x) const {
            // Assuming we are only going to call this with y <= 1.0, so r = 2.1 is enough (it should never be higher than 2)
            float l = 0, r = 2.1;

            // The loop iterates until a precision of 3 decimal places is reached
            while (abs(r - l) >= 1e-5) { 
                float mid = (l+r)/2;

                // Uses the function fast_log2 to find the adequate mean value
                // TODO: This is wrong for some reason... It does not work inside EPOS, but works outside of it.
                if (EPOS::S::U::Math::fast_log2(mid) > x)
                    r = mid;
                else
                    l = mid;
            }

            // Used for debugging
            EPOS::OStream cout;
            cout << "L: " << l << ", X: " << x << '\n';
            return l;
        }
};

#endif