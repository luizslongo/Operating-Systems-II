#include <utility/ostream.h>
#include <time.h>
#include <process.h>
#include <clerk.h>
#include <real-time.h>
#include "disparity.h"
//#include "bandwidth.h"
#include "cpu_hungry.h"
#include <utility/random.h>

using namespace EPOS;

OStream cout;

typedef TSC::Time_Stamp Time_Stamp;

// Configuration
const unsigned int  TEST_LENGTH            = 10; // in seconds
const bool          MEASURE_TIME           = false;
// To be measured
const float         MEMORY_IT_LENGHT       = 200;    // 0.4 in microseconds
const float         MEMORY_IT_LENGHT_T8    = 3;
const float         MEMORY_IT_LENGHT_T9    = 4;
const float         MEMORY2_IT_LENGHT      = 10;
const unsigned int  CPU_IT_LENGHT          = 1000;  // in microseconds
const unsigned int  MIDTERM_IT_LENGHT      = 120000;  // in microseconds

constexpr float TIMES[4] = { MEMORY_IT_LENGHT, CPU_IT_LENGHT, MIDTERM_IT_LENGHT, MEMORY2_IT_LENGHT};

// Simple task-set, just to monitor a variety of stress levels (varying memory/cpu/mix usage)
/* INTEL Version
* T \ C | 1       | 2       | 3       | 4       | 5       | 6       | 7       | 8
*  0 -> | 20      | 15      | 30      | 5       | 35      | 10      | 25      | 40
*  1 -> | 20      | 35      | 10      | 5       | 15      | 30      | 30      | 25
*  2 -> | 18      | 28      | 14      | 17.5    | 35      | 24      | 23.3    | 17.5
*  T -> | 58      | 78      | 54      | 27.5    | 85      | 64      | 78.3    | 82.5
*/

const unsigned int THREADS             = 7;//12;//8;//11;

// Turn around time das outras tem que aumentar
// verificar o tempo da segunda thread
// verificar o tempo do disparity da outra CPU
// verificar o tempo do disparity solo
// tirar a zero -->

constexpr static struct Task_Set {
    const unsigned int p;
    const unsigned int d;
    const unsigned int c;
    const unsigned int cpu;
    const unsigned int f;
} set[THREADS] = { //change order
    //PERIOD,DEADLINE,WCET,CPU,TASK
    //                                           thread 9 now has 60% usage
    {300000, 300000, 120000, 1, 2}, // 36   - 88540 ok (29.51)  + 20 = 49 | 1*89892 (29.7) = 49
    {300000, 300000, 120000, 2, 2}, // 66   - 2*64189  (42.83)  + 40 = 83 | 2*69231 (45.7) = 85 --> 42 + 20
    {300000, 300000, 120000, 3, 2}, // 84.5 - 7*36549  (85.281) + 0  = 85 | 7*35068 (81)        --> 
    //{300000, 300000, 240000, 4, 2}, // 72   - 63050    (42.03)  + 10 = 52 | 2*49606 (65.5) --> 98943 -- 33 + 10 TODO TEST  
    //{300000, 300000, 108000, 5, 2}, // 36   - 94841    (31.61)  + 0  = 31 | 1*95081 (31.4)
    //{300000, 300000, 253500, 6, 2}, // 84.5   7*36104  (84.243) + 0  = 84 | 7*34678 (80.1)
    //{300000, 300000, 198000, 7, 2}, // 72     60794    (40.5)   + 20 = 60 | 2*61464 (43) = 60 --> 55 TODO TEST
    //{300000, 300000, 198000, 8, 2}, // 66     2*64591  (43)     + 0  = 40 | 2*67463 (44.5)
    // Bandwidth
    {300000 ,300000 ,100000 ,1,0}, // 60 // 50 // 20
    {300000 ,300000 ,100000  ,4,0},  // 20
    //{100000 ,100000 ,30000 ,3,0},
    //{300000 ,300000 , 30000 ,4,0}, // 10
    //{200000 ,200000 ,70000 ,5,0},
    //{100000 ,100000 ,10000 ,6,0},
    //{300000 ,300000 , 45000 ,7,0}, // 15 // 20
    // CPU Affinity
    {300000, 300000, 100000, 4, 1},
    {300000, 300000, 100000, 3, 1}
    //{100000 ,100000 ,40000 ,8,0},
    // {100000 ,100000 ,20000 ,1,1},
    // {200000 ,200000 ,70000 ,2,1},
    // {100000 ,100000 ,10000 ,3,1},
    // {200000 ,200000 ,10000 ,4,1},
    // {200000 ,200000 ,30000 ,5,1},
    // {100000 ,100000 ,30000 ,6,1},
    // {100000 ,100000 ,30000 ,7,1},
    // {200000 ,200000 ,50000 ,8,1},
    // {1000000,1000000,140000,1,2},
    // {1000000,1000000,280000,2,2},
    // {1500000,1500000,210000,3,2},
    // {2000000,2000000,350000,4,2},
    // {1000000,1000000,350000,5,2},
    // {1000000,1000000,240000,6,2},
    // {1500000,1500000,350000,7,2},
    // {2000000,2000000,350000,8,2}
};


/* Think section
 * make thread [0] migrate every 10 executions to the other CPUS, print statistics at each migration
*/
unsigned int migrations_iters = 0;

inline constexpr int calc_iter_per_job(int i) {
    //if (i == 2-1)
    //    return set[i].c/TIMES[set[i].f] + 2;//4;
    //if (i == 3-1)
    //    return set[i].c/TIMES[set[i].f]; // roll back to 2 threads
    //if (i == 5-1)
    //    return set[i].c/TIMES[set[i].f] + 2; //4;
    //if (i == 8-1)
    //    return set[i].c/MEMORY_IT_LENGHT_T8; //TIMES[set[i].f];
    //if (i == 9-2)
    //    return set[i].c/MEMORY_IT_LENGHT_T9; //TIMES[set[i].f];
    //if (i == 9)
    //    return set[i].c/MEMORY_IT_LENGHT_T9;
    return set[i].c/TIMES[set[i].f];
}

inline constexpr int calc_jobs(int i) {
    return (TEST_LENGTH*1000000)/set[i].p;
}

template<int N>
struct ITER_PER_JOB {
    constexpr ITER_PER_JOB() : value() {
        for (int i = 0; i != N; ++i)
            value[i] = calc_iter_per_job(i); 
    }
    int value[N];
};

constexpr ITER_PER_JOB<THREADS> iter_per_job = ITER_PER_JOB<THREADS>();

template<int N>
struct JOBS {
    constexpr JOBS() : value() {
        for (int i = 0; i != N; ++i)
            value[i] = calc_jobs(i); 
    }
    int value[N];
};
constexpr JOBS<THREADS> jobs = JOBS<THREADS>();

unsigned int *g_mem_ptr[THREADS];           /* unsigned inter to allocated memory region */
//extern const unsigned int G_MEM_SIZE;
const unsigned int CACHE_LINE_SIZE = 64;
const unsigned int DEFAULT_ALLOC_SIZE_KB = 65536;
const unsigned int G_MEM_SIZE = DEFAULT_ALLOC_SIZE_KB;

inline Time_Stamp get_time() {
    return TSC::time_stamp();
}

inline Microsecond us(Time_Stamp ts) {
    return Convert::count2us<Hertz,Time_Stamp, Time_Base>(TSC::frequency(), ts);
}


unsigned int time_job[THREADS][20];

Time_Stamp diff[THREADS];
Time_Stamp wcet[THREADS];

extern signed char img1[];
extern signed char img2[];

signed char* t_img1[THREADS];
signed char* t_img2[THREADS];

//unsigned int iter;

unsigned int bench_write(unsigned int id)
{
    register unsigned int i;
    for ( i = 0; i < G_MEM_SIZE/sizeof(unsigned int); i+=(CACHE_LINE_SIZE/sizeof(unsigned int)) ) {
        g_mem_ptr[id][i] += i;
    }
    //for ( i = 0; i < G_MEM_SIZE/sizeof(unsigned int); i+=(CACHE_LINE_SIZE/sizeof(unsigned int)) ) {
    //    g_mem_ptr[i] = i;
    //}
    return G_MEM_SIZE;
}

int disparity(int id)
{
    int rows = 32;
    int cols = 32;
    I2D *imleft, *imright, *retDisparity;

    int WIN_SZ=8, SHIFT=64;

    imleft  = (I2D *) t_img1[id];
    imright = (I2D *) t_img2[id];

    rows = imleft->height;
    cols = imleft->width;

    (void)SHIFT;
    (void)WIN_SZ;

    retDisparity = getDisparity(imleft, imright, WIN_SZ, SHIFT);

    return retDisparity->height;
}

int mem_test() {
    static int count = 0;
    Random * rand;
    rand->seed(1);
    float ret = 1.33;
    int random = rand->random() % 3;
    cout << "rand%3=" << random << ",g7=" << g_mem_ptr[7] << ",g8=" << g_mem_ptr[8] << endl;
    //memcpy(reinterpret_cast<void *>(g_mem_ptr[7]),reinterpret_cast<void *>(g_mem_ptr[8]), G_MEM_SIZE);
    for (int i = 0; i < G_MEM_SIZE/sizeof(unsigned int); ++i)
    {
        g_mem_ptr[7][i] = g_mem_ptr[8][i];
    }
    count++;
    return int(ret);
}

template<unsigned int ID>
void run_func() {

    //cout << "INSIDE" << ID  << endl;
    if (ID == THREADS-1) // if thread 0
        migrations_iters++;
    //if (ID == 7)
    //    iter++;
    Time_Stamp init = 0;
    Time_Stamp end = 0;

    /*
    unsigned int iters = 0;
    // */

    unsigned int my_iter_per_job = iter_per_job.value[ID];
    unsigned int iterations;
    unsigned int ret;
    unsigned int cpu;

    // for(unsigned int job = 0; job < my_jobs; job++){
    //     Periodic_Thread::wait_next();
        cpu = CPU::id();
        init = get_time();

    //if (ID == THREADS-1) {
    //    cout << ID << "b"<< migrations_iters-1 <<"," << us(TSC::time_stamp()) << ",p=" << Thread::self()->priority() << endl;
    //}

        for(iterations = 0; iterations < my_iter_per_job; iterations++) {
            /*
            do { // remove when not measuring
                iters++;
            //*/
                switch(set[ID].f) {
                    case 0:
                        ret += bench_write(ID);//bandwidth(g_mem_ptr[ID]);
                        //ret += bandwidth(g_mem_ptr[cpu]);
                        break;
                    case 1:
                        ret += cpu_hungry();
                        break;
                    case 3:
                        ret += mem_test();
                        break;
                    default:
                        ret += disparity(ID);
                        break;
                }
            /* Remove when not measuring
                end = get_time();
            } while (end - init < TS_CONF[id][2]);
            //cout << "t=" << id << ",i=" << job << endl; // alive test
            // */
        }
        end = get_time();
        diff[ID] += end - init;
        if ((end - init)/my_iter_per_job > wcet[ID]){
            wcet[ID] = (end - init)/my_iter_per_job;
        }
        //time_job[id][job] = end - init; // Remove when not measuring
    //}
    cout << "job_done - " << ID << endl;

    /*
    if (MEASURE_TIME) {
        if (id > 15)
            iter_per_job[id] = iters/5;
        else
            iter_per_job[id] = iters/20;
    }

    */
    //if (ID == 7) {
    //    cout << ID << ",f" << iter-1 << "," << us(TSC::time_stamp()) << endl;
    //}
    //if (ID == THREADS-1) {
    //    cout << ID << ",f" << migrations_iters-1 << "," << us(TSC::time_stamp()) << endl;
    //}
    //if (!(migrations_iters % 30) && ID == THREADS-1 && migrations_iters <= 90) { // seems to lose deadlines
    //    cout << "MIGRATE" << endl;
        //Thread::self()->iwantmigrate = true;
    //}
}

Thread * threads[THREADS];

template<int ID>
inline void init_threads(Microsecond activation, TSC::Time_Stamp tsc0) {
    cout << ID << ",i=" << jobs.value[ID] << ",ij=" << iter_per_job.value[ID] << endl;
    threads[ID] = new RT_Thread(&run_func<ID>, set[ID].d, set[ID].p, set[ID].c, activation, jobs.value[ID], set[ID].cpu-1);
    //-us(TSC::time_stamp() - (tsc0 - activation))
    init_threads<ID + 1>(activation, tsc0);
};

template<>
inline void init_threads<THREADS>(Microsecond activation, TSC::Time_Stamp tsc0) {}

int main() // TODO: Add clock change in middle of execution (reschedule probably)
{
    cout << "Begin Main" << endl;
    //calc_iter_per_job();

    // SETUP data
    ///*
    for (unsigned int i = 0; i < THREADS; ++i)
    {
        if (set[i].f == 2) {
            t_img1[i] = new signed char[sizeof(img1)];
            t_img2[i] = new signed char[sizeof(img2)];
            for (int j = 0; j < sizeof(img1); ++j)
            {
                t_img1[i][j] = img1[j];
            }
            for (int j = 0; j < sizeof(img2); ++j)
            {
                t_img2[i][j] = img2[j];
            }
        } else if (set[i].f == 0) {
            g_mem_ptr[i] = new unsigned int[G_MEM_SIZE/sizeof(unsigned int)];
            cout << "g" << i << "=" << g_mem_ptr[i] << endl;
            for (unsigned int j = 0; j < G_MEM_SIZE / sizeof(unsigned int); j++) {
                g_mem_ptr[i][j] = j + j*i;
            }
        }
    }
    //*/

    // for (unsigned int i = 0; i < THREADS; i++)
    // {
    //     // RTConf(Period, deadline, capacity, activation, times, cpu, state, criterion), func, par1, par2
    //     // PEDF == Criterion(d, p, c, cpu)
    //     threads[i] = new Periodic_Thread(RTConf(TS_CONF[i][0], 0, TS_CONF[i][2], 0, jobs[i], (TS_CONF[i][3]-1), Thread::READY,
    //         Thread::Criterion(TS_CONF[i][0], TS_CONF[i][1],
    //         TS_CONF[i][2], (TS_CONF[i][3]-1))), &run_func, i, TS_CONF[i][4]);
    // }

    Time_Stamp tsc0 = get_time()+Convert::us2count<Time_Stamp, Time_Base>(TSC::frequency(),10000);

    init_threads<0>(10000, us(tsc0));

    Monitor::enable_captures(tsc0);
    //Monitor::disable_captures();
    cout << "All TASKs created" << "Time=" << us(tsc0) << endl;

    for (int i = 0; i < THREADS; ++i)
    {
        threads[i]->join();
        cout << "AVG-Monitor:" << Convert::count2us<Hertz, TSC::Time_Stamp, Time_Base>(TSC::frequency(), (threads[i]->_statistics.average_execution_time/threads[i]->_statistics.jobs)) << endl;
    }

    Monitor::disable_captures();

    Time_Stamp times = get_time() - tsc0;

    //printing thread ids
    cout << "Returned to application main!" << endl;

    cout << "Elapsed = " << us(times) << endl;
    cout << "Threads=" << THREADS << endl;

    cout << "-----------------------------------------------------" << endl;
    cout << "...............Threads Timing Behavior..............." << endl;
    cout << "-----------------------------------------------------" << endl;
    for (int i = 0; i < THREADS; ++i)
    {
        cout<<"t"<<i<<": "<<reinterpret_cast<unsigned int>(threads[i])<<endl;
        cout << "average execution time [" << i << "]: " << us(diff[i]/(jobs.value[i]*iter_per_job.value[i])) << ", diff: " << us(diff[i]) << endl;
        cout << "iter_per_job=" << iter_per_job.value[i] << "; ";
        cout << "WCET [" << i << "]: " << us(wcet[i]) << endl;
        if(MEASURE_TIME) {
            cout << "TIME_JOB:[";
            for (int j = 0; j < 20; ++j)
            {
                cout << "[" << j << "]" << time_job[i][j] << ",";
            }
            cout << "]" << endl;
        }
    }

    cout << "Threads_end" << endl;
    cout << "ID:" << reinterpret_cast<volatile unsigned int>(Thread::self());
    for (int i = 0; i < THREADS; ++i)
    {
        cout << "," << reinterpret_cast<volatile unsigned int>(threads[i]);
    }
    cout << "\n";

    return 0;
}

// Trash can
/*
void calc_iter_per_job() {
    for (int i = 0; i < THREADS; i++)
    {
        if (MEASURE_TIME) {
            jobs[i] = 20;
            if (TS_CONF[i][4] == 2)
                jobs[i] = 5;
            iter_per_job[i] = 1;
        } else {
            //if (i == 19) {
            //    iter_per_job[i] = TS_CONF[i][2]/35000;
            //} else {
                switch(TS_CONF[i][4]) { // [4] -> TASK
                                        // [2] ->  WCET
                    case 0:
                        cout << "0 => " << TS_CONF[i][2] << "*" << MEMORY_IT_LENGHT << " = " << TS_CONF[i][2] * MEMORY_IT_LENGHT <<endl; 
                        iter_per_job[i] = TS_CONF[i][2] * MEMORY_IT_LENGHT; // inverse relation
                        break;
                    case 1:
                        iter_per_job[i] = TS_CONF[i][2]/CPU_IT_LENGHT;
                        break;
                    default:
                        iter_per_job[i] = TS_CONF[i][2]/MIDTERM_IT_LENGHT;
                        break;
            //    }
            }
            if(iter_per_job[i] < 1) {
                iter_per_job[i] = 1;
            }
            jobs[i] = TEST_LENGTH*1000000/TS_CONF[i][0]; // [0] -> Period == Deadline
        }
        cout <<"jobs[" << i << "]: " << jobs[i] << ", Iters :" << iter_per_job[i] << endl;
    }
}
*/