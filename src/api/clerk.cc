// EPOS Clerk Implementation

#include <clerk.h>
#include <system.h>
#include <utility/string.h>
#include <utility/heap.h>

__BEGIN_SYS

extern "C" { extern unsigned int _sys_info; }
// Clerk
#ifdef __PMU_H

bool Clerk<PMU>::_in_use[Traits<Build>::CPUS][CHANNELS];
#ifdef __mach_pc__
constexpr PMU::Event Intel_Sandy_Bridge_PMU::_events[Intel_Sandy_Bridge_PMU::EVENTS];
#elif defined __arch_armv8__
constexpr CPU::Reg32 ARMv8_A_PMU::_events[PMU::EVENTS];
extern "C" {extern unsigned int __bss_end__;}
#endif
volatile bool Monitor::enable_injector[Traits<Build>::CPUS];
unsigned int **Monitor::anomalous_behaviour[Traits<Build>::CPUS];
unsigned int Monitor::samples[Traits<Build>::CPUS];
const float Monitor::SLOPE[6] = {0,0,0,0,0,0};
const float Monitor::STUCK[6] = {0,0,0,0,0,0};
const float Monitor::BIAS[6] = {0,0,0,0,0,0};
const float Monitor::GAIN[6] = {0,0,0,0,0,0};

#endif

// System_Monitor
Simple_List<Monitor> Monitor::_monitors[Traits<Build>::CPUS];
volatile bool Monitor::_enable;

bool Monitor::run()
{
    bool ret = true;
    if(_enable) { 
        db<Monitor>(TRC) << "Monitor::run()" << endl;
        Simple_List<Monitor> * monitor = &_monitors[CPU::id()];
        for(List::Iterator it = monitor->begin(); it != monitor->end(); it++)
            ret &= it->object()->capture();
    } else
        return false;
    return ret;
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
            db<Monitor>(WRN) << "Si Address: " << hex << si << endl;
            db<Monitor>(WRN) << "Si Address: " << hex << &__bss_end__ << endl;
            db<Monitor>(WRN) << "Monitor Cores: " << hex << si->bm.n_cpus << endl;
            // db<Monitor>(WRN) << "Monitor Extras: " << hex << si->bm.extras_offset << endl;
            db<Monitor>(WRN) << "Monitor Extras: " << hex << &__bss_end__ << endl;
            int checking_limit = 200000;
            char *text = (reinterpret_cast<char *>(&__bss_end__));
            char *aux = new char, *tmp = new char; //, *old = new char
            strcpy(tmp, "");
            // strcpy(old, "");
            strcpy(aux, "");
            const char *sign("fake_trace"); //this is the signature for arm... using anomalous_trace was breaking the system
            int i = 0;
            bool anomalous_trace = false;
            // db<Monitor>(WRN) << "Content EXTRA: " << i << " = " << text[i] << endl;
            for (; i < checking_limit && text[i] != ',' && text[i] != '\n'; i++);//  db<Monitor>(WRN) << "Content EXTRA: " << i << " = " << text[i] << endl; // jumps till first comma
            db<Monitor>(WRN) << "Find comma? " << i << " : "<< text[i] << endl;
            int subcounter = 0;
            for (; i < checking_limit && !anomalous_trace; i++) {
                subcounter += 1;
                // db<Monitor>(WRN) << "Content EXTRA: " << i << " = " << text[i] << endl;
                if (text[i] != ',' && text[i] != '\n' && subcounter < 13) {
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
                    subcounter = 0;
                }
            }
            // db<Monitor>(WRN) << "Content EXTRA: " << i << " = " << text[i] << endl;
            if(anomalous_trace) {
                // db<Monitor>(WRN) << "Monitor extrass Exists! " << i << endl;
                // gets size
                for (; i < checking_limit && text[i] != ',' && text[i] != '\n'; i++) {
                    *aux = text[i];
                    strcpy(&aux[1], "");
                    strcat(tmp, aux);
                }
                int nbytes = atoi(tmp);
                strcpy(tmp, "");
                // db<Monitor>(WRN) << "extras size: " << nbytes << endl;
                // si->bm.extras_offset = &text[i];
                unsigned int count = 0;
                unsigned int columns = 0, rows = 0;
                // db<Monitor>(WRN) << "define columns" << endl;
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
                                // db<Monitor>(WRN) << "CPU: "  << cpu << endl;
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
            // db<Monitor>(WRN) << "EXIT Extras! " << endl;
        }
    }
#elif defined __mmod_emote3__
    System_Info * si =  reinterpret_cast<System_Info *>(&_sys_info);
    if (si->bm.extras_offset != static_cast<unsigned int>(-1)) {
        db<Monitor>(WRN) << "Monitor Extras, #CPUS = " << hex << si->bm.n_cpus << endl;
        db<Monitor>(WRN) << "Monitor Extras, #CPUS = " << si->bm.n_cpus << endl;
        db<Monitor>(WRN) << "Monitor Extras, #should be at = " << hex << reinterpret_cast<unsigned int>(&_sys_info) << endl;
        db<Monitor>(WRN) << "Monitor Extras, #is at = " << hex << reinterpret_cast<unsigned int>(si) << endl;
    } else
        db<Monitor>(WRN) << "Monitor No Extras. "<< endl;
    db<Monitor>(WRN) << "Monitor Extras = " << hex << si->bm.extras_offset << endl;
    db<Monitor>(WRN) << "Monitor Extras = " << hex << si->bm.extras_offset+Traits<Machine>::APP_CODE+sizeof(int) << endl;
    char *text = (reinterpret_cast<char *>(si->bm.extras_offset + Traits<Machine>::APP_CODE+sizeof(int)));
    int nbytes = 0;
    int checking_limit = 2000;
    for (int k = 0; k < checking_limit && nbytes != 2449; k++) {
        nbytes = reinterpret_cast<int *>(si->bm.extras_offset+Traits<Machine>::APP_CODE)[k];
        db<Monitor>(WRN) << "Content size at " << k << ": " << nbytes << endl;
    }
    int i = 0;
    for (; i < checking_limit && text[i] != ',' && text[i] != '\n'; i++); // jumps till first comma
    checking_limit = i + 11;
    for (i = i+1; i < checking_limit; i++)
        db<Monitor>(WRN) << "Content EXTRA: " << i << " = " << text[i] << endl;
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

    // db<Monitor>(WRN) << "Monitor Initialized" << endl;
    // return;

}

__END_SYS
