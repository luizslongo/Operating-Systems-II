#ifndef __ieee1451_tim_h
#define __ieee1451_tim_h

#include <ieee1451_objects.h>
#include <condition.h>
#include <mutex.h>
#include <thread.h>
#include <ip.h>
#include <tcp.h>

__BEGIN_SYS

struct IEEE1451_TEDS_TIM
{
    IEEE1451_TEDS_TIM(char *payload, unsigned short size) : _payload(payload), _size(size) {}

    char *_payload;
    unsigned short _size;
};


class IEEE1451_Transducer
{
public:
    IEEE1451_Transducer();
    virtual ~IEEE1451_Transducer() {}

    void receive_msg(unsigned short trans_id, const char *message, unsigned int size);

protected:
    virtual IEEE1451_TEDS_TIM *get_teds(char id) = 0;
    //virtual bool running() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void read_data_set(unsigned short trans_id, unsigned int offset) = 0;
};


class IEEE1451_TIM //IEEE 1451.0 + IEEE 1451.5
{
private:
    IEEE1451_TIM();

public:
    ~IEEE1451_TIM();

    static IEEE1451_TIM *get_instance();

    void init_teds();
    IEEE1451_TEDS_TIM *get_teds(char id);
    void set_ncap_address(const IP::Address &addr) { _ncap_address = addr; }

    void connect();
    void disconnect();
    void receive_msg(unsigned short trans_id, const char *message, unsigned int size);
    void send_msg(unsigned short trans_id, unsigned int length);

    void set_transducer(IEEE1451_Transducer *transducer) { _transducer = transducer; }
    char *get_send_buffer() { _send_buffer_mutex.lock(); return _send_buffer + sizeof(IEEE1451_Packet); }

    void execute() { _receive_thread = new Thread(receive, this, &_channel); Alarm::delay(TIME_500_MS); }

private:
    static int receive(IEEE1451_TIM *tim, TCP::Channel *channel);

private:
    TCP::Channel _channel;
    Thread *_receive_thread;

    char *_send_buffer;
    Mutex _send_buffer_mutex;

    bool _connected;
    Condition _connected_cond;
    IP::Address _ncap_address;

    char *_meta_array;
    char *_tim_utn_array;
    char *_phy_array;
    IEEE1451_TEDS_TIM *_meta_teds;
    IEEE1451_TEDS_TIM *_tim_utn_teds;
    IEEE1451_TEDS_TIM *_phy_teds;

    IEEE1451_Transducer *_transducer;

    static IEEE1451_TIM *_ieee1451;
};

__END_SYS

#endif
