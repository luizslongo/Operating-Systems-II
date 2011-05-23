#ifndef __ieee1451_ncap_application_h
#define __ieee1451_ncap_application_h

#define USE_SIP

#include <ieee1451_ncap.h>
#include <ieee1451_objects.h>
#include <utility/list.h>
#ifdef USE_SIP
    #include <sip_defs.h>
    #include <sip_manager.h>
    #include <sip_user_agent.h>
#endif

__BEGIN_SYS

class IEEE1451_TEDS_Retriever;

class IEEE1451_NCAP_Application
{
private:
    friend class IEEE1451_TEDS_Retriever;

    struct TIM_Cache
    {
        IEEE1451_TIM_Channel *_tim;
        Simple_List<TIM_Cache>::Element _link;

#ifdef USE_SIP
        UserAgent *_ua;

        TIM_Cache(IEEE1451_TIM_Channel *tim, UserAgent *ua) : _tim(tim), _ua(ua), _link(this) {};
        ~TIM_Cache() { delete _tim; delete _ua; }
#else
        TIM_Cache(IEEE1451_TIM_Channel *tim) : _tim(tim), _link(this) {};
        ~TIM_Cache() { delete _tim; }
#endif
    };

    IEEE1451_NCAP_Application();

public:
    ~IEEE1451_NCAP_Application();

    static IEEE1451_NCAP_Application *get_instance();

    TIM_Cache *get_tim_cache(const IP::Address &address);
#ifdef USE_SIP
    TIM_Cache *get_tim_cache(const char *uri);
#endif
    IEEE1451_TEDS_Retriever *get_retriever(unsigned short trans_id);

    void update_tim(const IP::Address &address);
    void update_tim_completed(IEEE1451_TEDS_Retriever *retriever, IEEE1451_TIM_Channel *tim, IP::Address address);

    void report_tim_connected(const IP::Address &address);
    void report_tim_disconnected(const IP::Address &address);
    //void report_error(unsigned short trans_id, int error_code);
    void report_command_reply(const IP::Address &address, unsigned short trans_id, const char *message, unsigned int length);
    void report_tim_initiated_message(const IP::Address &address, const char *message, unsigned int length);

    void read_temperature(const IP::Address &address, const char *buffer);

    unsigned short send_operate(const IP::Address &address, unsigned short channel_number);
    unsigned short send_idle(const IP::Address &address, unsigned short channel_number);
    unsigned short send_read_teds(const IP::Address &address, unsigned short channel_number, char tedsId);
    unsigned short send_read_data_set(const IP::Address &address, unsigned short channel_number);

#ifdef USE_SIP
    void send_sip_message(UserAgent *ua, const char *data);
    void send_sip_notify(UserAgent *ua, SipSubscriptionState state, SipPidfXmlBasicElement pidfXml);
    static int message_callback(SipEventCallback event, UserAgent *ua, const char *remote);
#endif

    //static int read_data_set_thread(IEEE1451_NCAP_Application *ncap, IP::Address address, IEEE1451_TIM_Channel *tim);

private:
    Simple_List<TIM_Cache> _cache;
    Simple_List<IEEE1451_TEDS_Retriever> _retrievers;

    static IEEE1451_NCAP_Application *_application;
};


class IEEE1451_TEDS_Retriever
{
private:
    friend class IEEE1451_NCAP_Application;

    enum State
    {
        meta_teds = 0,
        tim_transducer_name_teds = 1,
        phy_teds = 2,
        transducer_channel_teds = 3,
        transducer_name_teds = 4
    };

public:
    IEEE1451_TEDS_Retriever(const IP::Address &address, IEEE1451_NCAP_Application *application);
    ~IEEE1451_TEDS_Retriever() {};

    void repeat() { execute(); }
    void process(const char *message, unsigned int length);
    void execute();

private:
    IP::Address _address;
    IEEE1451_NCAP_Application *_application;
    IEEE1451_Transducer_Channel *_transducer;
    IEEE1451_TIM_Channel *_tim;

    short _state;
    char _teds_id;
    unsigned short _last_trans_id;

    Simple_List<IEEE1451_TEDS_Retriever>::Element _link;
};

__END_SYS

#endif
