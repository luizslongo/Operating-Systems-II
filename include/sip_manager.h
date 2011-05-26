#ifndef __sip_manager_h
#define __sip_manager_h

#include <sip_defs.h>
#include <sip_message.h>
#include <mutex.h>
#include <ip.h>
#include <udp.h>
#include <utility/list.h>

__BEGIN_SYS

#define SIP_PORT 5060

class SIP_Transport_Layer
{
public:
    SIP_Transport_Layer() : _udp(IP::instance()), _socket(&_udp) { _host_ip = 0; _host_port = 0; _send_buffer[0] = 0; }
    ~SIP_Transport_Layer() { if (_host_ip) delete _host_ip; }

    bool init();

    int send_message(SIP_Message *msg);
    int send_data(const char *destination, int port, const char *data, int length);

    const char *get_host_ip() { return _host_ip; }
    int get_host_port() { return _host_port; }

private:
    class My_Socket : public UDP::Socket
    {
    public:
        My_Socket(UDP *udp) : UDP::Socket(udp, UDP::Address(IP::instance()->address(), SIP_PORT), UDP::Address(Traits<IP>::BROADCAST, SIP_PORT)) {}
        virtual ~My_Socket() {}

        void received(const UDP::Address &src, const char *data, unsigned int size);
    };

private:
    char *_host_ip;
    int _host_port;

    char _send_buffer[MAX_MSG_SIZE + 1];
    UDP _udp;
    My_Socket _socket;
};


class SIP_User_Agent;
typedef int (*USER_CALLBACK)(SIP_Event_Callback event, SIP_User_Agent *ua, const char *remote);

class SIP_Manager
{
private:
    SIP_Manager();

public:
    ~SIP_Manager();

    static SIP_Manager *get_instance();
    static void delete_instance();
    //static bool is_terminated() { return _terminated; }

    bool init();

    static int receive_message_thread();

    static USER_CALLBACK _callback;
    static void register_user_handler(USER_CALLBACK callback) { SIP_Manager::_callback = callback; }

    SIP_User_Agent *create_user_agent(const char *uri);
    SIP_User_Agent *get_user_agent(const char *uri);

    SIP_Transport_Layer *get_transport() { return &_transport; }

    void add_message_list(SIP_Message *msg);
    SIP_Message *get_message_list();

    static void random(char *buffer);

private:
    Simple_List<SIP_Message> _messages;
    Mutex _messages_mutex;

    Thread *_thread_receive_message;

    SIP_Transport_Layer _transport;
    Simple_List<SIP_User_Agent> _ua;

    static bool _terminated;
    static SIP_Manager *_instance;
};

__END_SYS

#endif
