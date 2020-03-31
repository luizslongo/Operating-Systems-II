// EPOS Clerk Implementation

#include <clerk.h>
#include <system.h>
#include <utility/string.h>
#include <utility/heap.h>

__BEGIN_SYS

// Clerk
#ifdef __PMU_H

bool Clerk<PMU>::_in_use[Traits<Build>::CPUS][CHANNELS];
#ifdef __mach_pc__
constexpr PMU::Event Intel_Sandy_Bridge_PMU::_events[Intel_Sandy_Bridge_PMU::EVENTS];
#elif defined __arch_armv8__
constexpr CPU::Reg32 ARMv8_A_PMU::_events[PMU::EVENTS];
#endif
bool Monitor::enable_injector[Traits<Build>::CPUS];
unsigned int **Monitor::anomalous_behaviour[Traits<Build>::CPUS];
unsigned int Monitor::samples[Traits<Build>::CPUS];
const float Monitor::SLOPE = 0;
const float Monitor::STUCK = 0;
const float Monitor::BIAS = 0;
const float Monitor::GAIN = 0;

#endif

// System_Monitor
Simple_List<Monitor> Monitor::_monitors[Traits<Build>::CPUS];
volatile bool Monitor::_enable;

void Monitor::run()
{
    if(_enable) { 
        db<Monitor>(TRC) << "Monitor::run()" << endl;
        Simple_List<Monitor> * monitor = &_monitors[CPU::id()];
        for(List::Iterator it = monitor->begin(); it != monitor->end(); it++)
            it->object()->capture();
    }
}

void Monitor::init()
{
    db<Monitor>(TRC) << "Monitor::init()" << endl;
#ifdef __mach_pc__
    if (CPU::id() == 0) {
        for (unsigned int i = 0; i < Traits<Build>::CPUS; i++) {
            samples[i] = 0;
            enable_injector[i] = false;
        }
        System_Info * si = System::info();
        if (si->bm.extras_offset != static_cast<unsigned int>(-1)) {
            db<Monitor>(WRN) << "Monitor Extras: " << hex << si->pmm.ext_base << endl;
            int nbytes = reinterpret_cast<int *>(si->pmm.ext_base)[0];
            char *text = (reinterpret_cast<char *>(si->pmm.ext_base+sizeof(int)));
            char *tmp = new char, *old = new char, *aux = new char;
            strcpy(tmp, "");
            strcpy(old, "");
            strcpy(aux, "");
            int i = 0;
            bool anomalous_trace = false;
            db<Monitor>(WRN) << "Content EXTRA: "  << nbytes << endl;
            for (; i < nbytes; i++) {
                // db<Monitor>(WRN) << "Content EXTRA: " << i << " = " << text[i] << endl;
                if (text[i] != ',' && text[i] != '\n') {
                    // db<Monitor>(WRN) << "old: " << old << ", tmp: " << tmp << " ,aux: " << aux << endl;
                    *aux = text[i];
                    strcpy(&aux[1], ""); // this is necessary to clean the trash from the previous instruction
                    strcat(tmp, aux);
                    // strcpy(old, tmp);
                } else {
                    const char *sign("anomalous_trace"); //this is the signature
                    if (strcmp(tmp, sign) == 0) {
                        anomalous_trace = true;
                        strcpy(tmp, "");
                        // strcpy(old, "");
                        strcpy(aux, "");
                        break;
                    }
                }
            }
            if(anomalous_trace) {
                // db<Monitor>(WRN) << "Anomalous Trace" << endl;
                unsigned int count = 0;
                unsigned int columns = 0, rows = 0;
                // db<Monitor>(WRN) << "define columns" << endl;
                for (i = i+1; i < nbytes; i++) {
                    if (text[i] != ',' && text[i] != '\n') {
                        *aux = text[i];
                        strcpy(&aux[1], "");
                        strcat(tmp, aux);
                        // strcpy(old, tmp);
                    } else {
                        columns = atoi(tmp);
                        strcpy(tmp, "");
                        // strcpy(old, "");
                        strcpy(aux, "");
                        break;
                    }
                }
                // db<Monitor>(WRN) << "define rows" << endl;
                for (i = i+1; i < nbytes; i++) {
                    if (text[i] != ',' && text[i] != '\n') {
                        *aux = text[i];
                        strcpy(&aux[1], "");
                        strcat(tmp, aux);
                        // strcpy(old, tmp);
                    } else {
                        rows = atoi(tmp);
                        strcpy(tmp, "");
                        // strcpy(old, "");
                        strcpy(aux, "");
                        break;
                    }
                }
                // db<Monitor>(WRN) << "columns: "  << columns << ", rows: " << rows << endl;
                int cpu = -1;
                for (i = i+1; i < nbytes; i++) {
                    if (text[i] != ',' && text[i] != '\n') {
                        *aux = text[i];
                        strcpy(&aux[1], "");
                        strcat(tmp, aux);
                        // strcpy(old, tmp);
                    } else {
                        if (cpu == -1) {
                            cpu = atoi(tmp);
                            strcpy(tmp, "");
                            // strcpy(old, "");
                            strcpy(aux, "");
                            anomalous_behaviour[cpu] = new unsigned int*[rows];
                            for (unsigned int j = 0; j < rows; j++) {
                                anomalous_behaviour[cpu][j] = new unsigned int[columns];
                            }
                            samples[cpu] = rows;
                        } else {
                            // **Monitor::anomalous_behaviour[Traits<Build>::CPUS];
                            // db<Monitor>(WRN) << "position: "  << count/columns << "," << count%columns << endl;
                            anomalous_behaviour[cpu][count/columns][count%columns] = atoi(tmp);
                            // db<Monitor>(WRN) << "cpu: "<< cpu << "value: "  << anomalous_behaviour[cpu][count/columns][count%columns] << endl;
                            strcpy(tmp, "");
                            // strcpy(old, "");
                            strcpy(aux, "");
                            count++;
                            if (count >= columns*rows) {
                                count = 0;
                                cpu = -1;
                            }
                        }
                    }
                }
                delete tmp;
                // delete old;
                delete aux;
                delete text;
                //added a ',' after numbers...
                /*
                db<Monitor>(WRN) << "position: "  << count/columns << "," << count%columns << endl;
                db<Monitor>(WRN) << "value: "  << atoi(tmp) << endl;
                tmp = new char;
                old = new char;
                */
            }
        }
    }
#elif defined __mmod_raspberry_pi3__
    if (CPU::id() == 0) {
        for (unsigned int i = 0; i < Traits<Build>::CPUS; i++) {
            samples[i] = 0;
            enable_injector[i] = false;
        }
        System_Info * si = System::info();
        if (si->bm.extras_offset != static_cast<unsigned int>(-1)) {
            db<Monitor>(WRN) << "Monitor Extras: " << hex << si->bm.n_cpus << endl;
            db<Monitor>(WRN) << "Monitor Extras: " << hex << si->bm.extras_offset << endl;
            db<Monitor>(WRN) << "Monitor Extras: " << hex << si->bm.extras_offset + Traits<Machine>::APP_CODE << endl;
            int checking_limit = 1000;
            char *text = (reinterpret_cast<char *>(si->bm.extras_offset + Traits<Machine>::APP_CODE+sizeof(int)));
            char *aux = new char, *tmp = new char; //, *old = new char
            strcpy(tmp, "");
            // strcpy(old, "");
            strcpy(aux, "");
            const char *sign("fake_trace"); //this is the signature for arm... using anomalous_trace was breaking the system
            int i = 0;
            bool anomalous_trace = false;
            db<Monitor>(WRN) << "Content EXTRA: " << i << " = " << text[i] << endl;
            for (; i < checking_limit && text[i] != ',' && text[i] != '\n'; i++); // jumps till first comma
            for (; i < checking_limit && !anomalous_trace; i++) {
                db<Monitor>(WRN) << "Content EXTRA: " << i << " = " << text[i] << endl;
                if (text[i] != ',' && text[i] != '\n') {
                    // db<Monitor>(WRN) << "old: " << old << ", tmp: " << tmp << " ,aux: " << aux << endl;
                    *aux = text[i];
                    strcpy(&aux[1], ""); // this is necessary to clean the trash from the previous instruction
                    // size_t t_len = strlen(tmp);
                    // size_t a_len = strlen(aux);
                    // db<Monitor>(WRN) << "LEN: " << t_len << " = " << a_len << endl;
                    // strncpy(&tmp[t_len], &aux[0], a_len);
                    // strcpy(&tmp[t_len+1], "");
                    strcat(tmp, aux);
                    // strcpy(old, tmp);
                } else {
                    if (strcmp(tmp, sign) == 0) {
                        anomalous_trace = true;
                        strcpy(tmp, "");
                    } else {
                        strcpy(tmp, "");
                    }
                }
            }
            db<Monitor>(WRN) << "Content EXTRA: " << i << " = " << text[i] << endl;
            if(anomalous_trace) {
                db<Monitor>(WRN) << "Monitor Extras! " << i << endl;
                // gets size
                for (; i < checking_limit && text[i] != ',' && text[i] != '\n'; i++) {
                    *aux = text[i];
                    strcpy(&aux[1], "");
                    strcat(tmp, aux);
                }
                int nbytes = atoi(tmp);
                strcpy(tmp, "");
                db<Monitor>(WRN) << "extras size: " << nbytes << endl;
                // si->bm.extras_offset = &text[i];
                unsigned int count = 0;
                unsigned int columns = 0, rows = 0;
                db<Monitor>(WRN) << "define columns" << endl;
                nbytes += i;
                for (i = i+1; i < nbytes; i++) {
                    if (text[i] != ',' && text[i] != '\n') {
                        *aux = text[i];
                        strcpy(&aux[1], "");
                        strcat(tmp, aux);
                        // strcpy(old, tmp);
                    } else {
                        columns = atoi(tmp);
                        strcpy(tmp, "");
                        // strcpy(old, "");
                        strcpy(aux, "");
                        break;
                    }
                }
                db<Monitor>(WRN) << "define rows" << endl;
                for (i = i+1; i < nbytes; i++) {
                    if (text[i] != ',' && text[i] != '\n') {
                        *aux = text[i];
                        strcpy(&aux[1], "");
                        strcat(tmp, aux);
                        // strcpy(old, tmp);
                    } else {
                        rows = atoi(tmp);
                        strcpy(tmp, "");
                        // strcpy(old, "");
                        strcpy(aux, "");
                        break;
                    }
                }
                db<Monitor>(WRN) << "columns: "  << columns << ", rows: " << rows << endl;
                int cpu = -1;
                for (i = i+1; i < nbytes; i++) {
                    if (text[i] != ',' && text[i] != '\n') {
                        *aux = text[i];
                        strcpy(&aux[1], "");
                        strcat(tmp, aux);
                        // strcpy(old, tmp);
                    } else {
                        if (cpu == -1) {
                            cpu = atoi(tmp);
                            strcpy(tmp, "");
                            // strcpy(old, "");
                            // strcpy(aux, "");
                            anomalous_behaviour[cpu] = new unsigned int*[rows];
                            for (unsigned int j = 0; j < rows; j++) {
                                anomalous_behaviour[cpu][j] = new unsigned int[columns];
                            }
                            samples[cpu] = rows;
                        } else {
                            // **Monitor::anomalous_behaviour[Traits<Build>::CPUS];
                            // db<Monitor>(WRN) << "position: "  << count/columns << "," << count%columns << endl;
                            anomalous_behaviour[cpu][count/columns][count%columns] = atoi(tmp);
                            // db<Monitor>(WRN) << "cpu: "<< cpu << "value: "  << anomalous_behaviour[cpu][count/columns][count%columns] << endl;
                            strcpy(tmp, "");
                            // strcpy(old, "");
                            // strcpy(aux, "");
                            count++;
                            if (count >= columns*rows) {
                                db<Monitor>(WRN) << "CPU: "  << cpu << endl;
                                count = 0;
                                cpu = -1;
                            }
                        }
                    }
                }
            }
            delete text;
            delete tmp;
            delete aux;
            // delete si;
            delete sign;
            db<Monitor>(WRN) << "EXIT Extras! " << endl;
        }
    }
#elif defined __mmod_emote3__
    System_Info * si = System::info();
    if (si->bm.extras_offset != static_cast<unsigned int>(-1)) {
        db<Monitor>(WRN) << "Monitor Extras: " << hex << si->bm.n_cpus << endl;
    } else
        db<Monitor>(WRN) << "Monitor No Extras. "<< endl;
#endif


#ifdef __mmod_emote3__
    return;
#endif

#ifdef __PMU_H

    if(Traits<PMU>::enabled && Traits<PMU>::monitored)
        init_pmu_monitoring<0>();

#endif

    if(Traits<System>::monitored)
        init_system_monitoring<0>();

    db<Monitor>(WRN) << "Monitor Initialized" << endl;
    // return;

}

__END_SYS
