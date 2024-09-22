#ifndef THREAD_H
#define THREAD_H

#include<utility/queue.h>
#include<utility/math.h>

/**
deadline
tempo de criação
tipo de tarefa --> CRITICAL, MEDIUM, BEST_EFFORT
tempo de começo - start_time

 */

struct ThreadArgs {
    int id;
    int type;
    unsigned int deadline;
    unsigned int creation_time;
    unsigned int task_time;
};

class Thread {
    public:
        enum type : int {
            CRITICAL   = 0,
            BEST_EFFORT = 1
        };
        Thread(ThreadArgs& args) :_id{args.id}, 
                                  _type(args.type), 
                                  _execution_time(0), 
                                  _creation_time(args.creation_time),
                                  _deadline(args.deadline), 
                                  _element(this, args.deadline | ( args.type != CRITICAL ?  1 << (sizeof(unsigned int) - 1) : 0 )),
                                  _task_time(args.task_time) {};

        int type();
        int id();
        EPOS::Ordered_Queue<Thread>::Element& element();
        unsigned int deadline();
        unsigned int creation_time();
        bool finished(); //task_time >= execution_time
        double new_frequency(unsigned int current_time, double min_cpu_frequency, double max_cpu_frequency);
        
        unsigned int execute(unsigned int current_time);
    
    private:
        int _id;
        int _type;
        unsigned int _execution_time;
        unsigned int _creation_time;
        unsigned int _deadline;
        EPOS::Ordered_Queue<Thread>::Element _element;
        unsigned int _task_time;
        
        int round(double x) const {
            return x - (int) x < 0.5 ? (int) x : (int) x + 1;
        }
        
        double abs(double x) const {
            return x < 0 ? -x : x;
        }

        // FROM: https://stackoverflow.com/a/29861395
        double pow(int x, double y) const {
            if (x < 0 and abs(round(y)-y) < 1e-8) {
                return pow(-x, y) * ((int)round(y)%2==1 ? -1 : 1);
            } else if (y < 0) {
                return 1/pow(x, -y);
            } else if(y > 1) {
                return pow(x * x, y / 2);
            } else {
                double fraction = 1;
                double result = 1;

                while(y > 1e-8) {
                    if (y >= fraction) {
                        y -= fraction;
                        result *= x;
                    }

                    fraction /= 2;
                    x = EPOS::Math::sqrt(x);
                }
                return result;
            }
        }
};

#endif