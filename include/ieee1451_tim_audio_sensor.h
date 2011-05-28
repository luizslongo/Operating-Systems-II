#ifndef __ieee1451_tim_audio_sensor_h
#define __ieee1451_tim_audio_sensor_h

#include <ieee1451_tim.h>
//#include <mutex.h>
#include <thread.h>

#define DATASET_SIZE    160
#define READ_INTERVAL   6000000 //60s

__BEGIN_SYS

class IEEE1451_Audio_Sensor : public IEEE1451_Transducer
{
public:
    IEEE1451_Audio_Sensor(bool tim_im, bool polling);
    ~IEEE1451_Audio_Sensor();

protected:
    void init_teds();
    IEEE1451_TEDS_TIM *get_teds(char id);
    //bool running() { return run; };
    void start();
    void stop();
    void read_data_set(unsigned short trans_id, unsigned int offset);
    void send_data_set(bool first = false, bool last = false);

public:
    int execute();

private:
    unsigned short _channel_number;

    bool _tim_im;
    bool _polling;

    static char _data_set[][DATASET_SIZE];
    //Mutex _data_set_mutex;
    int _pos;

    Thread *_execute_thread;

    char *_channel_array;
    char *_temp_sensor_utn_array;
    IEEE1451_TEDS_TIM *_channel_teds;
    IEEE1451_TEDS_TIM *_temp_sensor_utn_teds;
};

__END_SYS

#endif
