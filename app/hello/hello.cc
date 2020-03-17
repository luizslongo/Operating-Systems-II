#include <utility/ostream.h>
#include <clerk.h>
#include <time.h>
#include <real-time.h>
using namespace EPOS;

// #define SAMPLES 50
// #define CHANNELS 4
OStream cout;

const unsigned int MAX_EXECS = 1;

// // img1 and img2 size dependent
const unsigned int PERIOD   = 6000000; // IA32 == 160000
const unsigned int DEADLINE = 6000000; // IA32 == 160000
const unsigned int WCET     = 5700000; // IA32 == 155000

typedef TSC::Time_Stamp Time_Stamp;

// static unsigned int data[SAMPLES][CHANNELS] = {
//     {25034798,5498554,1944340,7687},
//     {20276095,5071377,32822,4950},
//     {19542535,4887898,35201,4774},
//     {27442367,5760171,2785146,9002},
//     {19181935,4797704,33301,4686},
//     {19223323,4808056,36725,4699},
//     {22299081,4994330,1501332,6635},
//     {20147682,5039258,37430,4920},
//     {19962438,4992925,33137,4876},
//     {27889839,5553122,3557854,9815},
//     {19956237,4991374,35875,4873},
//     {19534883,4885984,35881,4772},
//     {9212187,847741,3614207,5275},
//     {38343479,9590389,62625,9361},
//     {19603227,4903078,38227,4779},
//     {24920131,5463383,1962751,7688},
//     {20532465,5135501,30436,5013},
//     {19399525,4852128,37661,4742},
//     {26166142,5311455,3067661,9033},
//     {20324123,5083390,34894,4954},
//     {20043943,5013311,41098,4886},
//     {23288077,4892737,2360093,7653},
//     {19345166,4838532,37274,4719},
//     {20007537,5004205,36819,4878},
//     {8065906,743181,3175524,4682},
//     {38003353,9505316,68164,9276},
//     {19579581,4897164,37709,4774},
//     {26209884,5605161,2397170,8432},
//     {19899485,4977179,34602,4859},
//     {19925093,4983584,36616,4871},
//     {21079940,5057171,603288,5569},
//     {19647569,4914169,36341,4799},
//     {20400973,5102612,35588,4984},
//     {21477682,5014880,916940,6016},
//     {20286749,5074042,36404,4956},
//     {19413811,4855701,34906,4743},
//     {21542077,4929813,1170877,6141},
//     {19559831,4892224,34385,4781},
//     {19199575,4802116,35764,4692},
//     {27951903,5514542,3692425,9919},
//     {20508602,5129532,33356,5007},
//     {19422728,4857932,34437,4741},
//     {20003349,5002265,37526,4894},
//     {19722558,4932924,37058,4810},
//     {19158342,4791801,35377,4670},
//     {21834683,5268201,526437,5731},
//     {20248228,5064405,36588,4936},
//     {19667404,4919129,36859,4796},
//     {20372819,4886743,573909,5392},
//     {19798706,4951970,35715,4834}};

void FuncTask1()
{

    Monitor::enable_injector[0] = false;
    for(int i = 0; i < 0xfff; i++)
        cout<<"Hello World"<<endl;
}

void FuncTask2()
{
    // Monitor::anomalous_behaviour[1] = new unsigned int*[SAMPLES]; //[CHANNELS];
    // for (unsigned int j = 0; j < SAMPLES; j++) {
    //     Monitor::anomalous_behaviour[1][j] = new unsigned int[CHANNELS];
    // }
    // for (unsigned int i = 0; i < SAMPLES; i++) {
    //     for (unsigned int j = 0; j < CHANNELS; j++) {
    //         Monitor::anomalous_behaviour[1][i][j] = data[i][j];
    //     }
    // }
    // Monitor::samples[1] = SAMPLES;
    Monitor::enable_injector[1] = true;
}

void FuncTask3()
{
    Monitor::enable_injector[2] = true;
}

void FuncTask4()
{
    Monitor::enable_injector[3] = true;
}

void FuncTask5()
{
    Monitor::enable_injector[4] = true;
}

void FuncTask6()
{
    Monitor::enable_injector[5] = true;
}

void FuncTask7()
{
    Monitor::enable_injector[6] = true;
}

void FuncTask8()
{
    Monitor::enable_injector[7] = true;
}

int main()
{
    cout << "entry main" << endl;
    Monitor::enable_injector[0] = false;
    cout << "init thread" << endl;
    // Time_Stamp tsc0 = TSC::time_stamp()+Convert::us2count<Time_Stamp, Time_Base>(TSC::frequency(),10000);
    cout << "Ptr: " <<  Monitor::anomalous_behaviour[0] << endl;
    cout << "Ptr: " <<  hex << Monitor::anomalous_behaviour[1] << ", " << sizeof(Monitor::anomalous_behaviour[1]) << endl;
    cout << "Ptr: " <<  hex << Monitor::anomalous_behaviour[3][49] << endl;
    cout << "new rt_thread" << endl;

    Thread * t = new RT_Thread(&FuncTask1, DEADLINE, PERIOD, WCET, Periodic_Thread::NOW, MAX_EXECS);

    cout << "Disparity Periodic: All TASKs created with result (>0 is OK): " << t << endl;
    // cout << "Disparity Periodic: All TASKs created with result (>0 is OK): " << endl;

    Thread *secondary[7];
    secondary[0] = new RT_Thread(&FuncTask2, DEADLINE, PERIOD, WCET, Periodic_Thread::NOW, MAX_EXECS);
    secondary[1] = new RT_Thread(&FuncTask3, DEADLINE, PERIOD, WCET, Periodic_Thread::NOW, MAX_EXECS);
    secondary[2] = new RT_Thread(&FuncTask4, DEADLINE, PERIOD, WCET, Periodic_Thread::NOW, MAX_EXECS);
    secondary[3] = new RT_Thread(&FuncTask5, DEADLINE, PERIOD, WCET, Periodic_Thread::NOW, MAX_EXECS);
    secondary[4] = new RT_Thread(&FuncTask6, DEADLINE, PERIOD, WCET, Periodic_Thread::NOW, MAX_EXECS);
    secondary[5] = new RT_Thread(&FuncTask7, DEADLINE, PERIOD, WCET, Periodic_Thread::NOW, MAX_EXECS);
    secondary[6] = new RT_Thread(&FuncTask8, DEADLINE, PERIOD, WCET, Periodic_Thread::NOW, MAX_EXECS);

    Monitor::enable_captures();

    for(int i = 0; i < 0xfff; i++)
        cout<<"Hello World"<<endl;

    // t->join();
    secondary[0]->join();
    secondary[1]->join();
    secondary[2]->join();
    secondary[3]->join();
    secondary[4]->join();
    secondary[5]->join();
    secondary[6]->join();
    // Time_Stamp tsc0 = TSC::time_stamp()+Convert::us2count<Time_Stamp, Time_Base>(TSC::frequency(),10000);
    // Monitor::enable_captures(tsc0);
    // cout << "MAIN ENTRY" << endl;
    //
    // cout << "Convert Samples" << endl;
    // // unsigned int *aux[SAMPLES];
    // // for (unsigned int i = 0; i < SAMPLES; i++) {
    // //     aux[i] = data[i];
    // // }
    // cout << "Configuring Monitor" << endl;
    // Monitor::anomalous_behaviour = new unsigned int*[SAMPLES]; //[CHANNELS];
    // for (unsigned int j = 0; j < SAMPLES; j++) {
    //     Monitor::anomalous_behaviour[j] = new unsigned int[CHANNELS];
    // }
    // for (unsigned int i = 0; i < SAMPLES; i++) {
    //     for (unsigned int j = 0; j < CHANNELS; j++) {
    //         Monitor::anomalous_behaviour[i][j] = data[i][j];
    //     }
    // }
    // // *Monitor::anomalous_behaviour = *aux;
    // Monitor::samples = SAMPLES;
    // Monitor::enable_injector = true ;
    //
    // for (int i = 0; i < 0xfff; i++)
    //     cout << "Hello world!" << endl;

    return 0;
}
