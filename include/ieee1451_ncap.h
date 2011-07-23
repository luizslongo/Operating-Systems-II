#ifndef __ieee1451_ncap_h
#define __ieee1451_ncap_h

#include <ieee1451_objects.h>
#include <mutex.h>
#include <thread.h>
#include <ip.h>
#include <tcp.h>
#include <utility/list.h>
#include <utility/malloc.h>

__BEGIN_SYS

struct IEEE1451_TEDS_NCAP
{
    IEEE1451_TEDS_NCAP(char id, const char *teds, unsigned short length, bool sub_block = false);
    ~IEEE1451_TEDS_NCAP() { kfree(_value); }

    char *get_tlv(char type);

    char _id;
    unsigned short _length;
    char *_value;

    Simple_List<IEEE1451_TEDS_NCAP>::Element _link;
};


struct IEEE1451_Channel
{
    virtual ~IEEE1451_Channel();

    IEEE1451_TEDS_NCAP *get_teds(char id);
    void add_teds(IEEE1451_TEDS_NCAP *teds) { _teds.insert(&teds->_link); }

    IP::Address _address;
    Simple_List<IEEE1451_TEDS_NCAP> _teds;
};

struct IEEE1451_Transducer_Channel : public IEEE1451_Channel
{
    unsigned short _channel_number;
};

struct IEEE1451_TIM_Channel : public IEEE1451_Channel
{
    IEEE1451_TIM_Channel() : _transducer(0), _connected(false) {}
    ~IEEE1451_TIM_Channel() { if (_transducer) delete _transducer; }

    IEEE1451_Transducer_Channel *_transducer;
    bool _connected;
};

class Linked_Channel : public TCP::Channel
{
public:
    Linked_Channel() : _link(this) {}

    Simple_List<Linked_Channel>::Element _link;
};

class IEEE1451_NCAP //IEEE 1451.0 + IEEE 1451.5
{
private:
    IEEE1451_NCAP();

public:
    ~IEEE1451_NCAP();

    static IEEE1451_NCAP *get_instance();

    unsigned short send_command(const IP::Address &destination, unsigned short channel_number, unsigned short command, const char *args = 0, unsigned int length = 0);
    void execute();

private:
    Linked_Channel *get_channel(const IP::Address &addr);
    static int receive(IEEE1451_NCAP *ncap, Linked_Channel *channel);
    static void cleaner(Thread *thread) { delete thread; }

public:
    class Listener
    {
    public:
        Listener() {}
        virtual ~Listener() {}

        virtual void report_tim_connected(const IP::Address &address) = 0;
        virtual void report_tim_disconnected(const IP::Address &address) = 0;
        //virtual void report_error(unsigned short trans_id, int error_code) = 0;
        virtual void report_command_reply(const IP::Address &address, unsigned short trans_id, const char *message, unsigned int length) = 0;
        virtual void report_tim_initiated_message(const IP::Address &address, const char *message, unsigned int length) = 0;
    } *_application;

    void set_application(Listener *application) { _application = application; }

private:
    Simple_List<Linked_Channel> _channels;
    char *_send_buffer;
    Mutex _send_buffer_mutex;

    unsigned int id_generator;

    static IEEE1451_NCAP *_ieee1451;
};

__END_SYS

#endif
