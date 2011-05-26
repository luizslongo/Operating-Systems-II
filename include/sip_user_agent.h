#ifndef __sip_user_agent_h
#define __sip_user_agent_h

#include <sip_defs.h>
#include <sip_header.h>
#include <sip_message.h>
#include <sip_transactions.h>
#include <utility/list.h>

__BEGIN_SYS

class SIP_Dialog
{
private:
    friend class SIP_User_Agent;
    friend class SIP_User_Agent_Client;
    friend class SIP_User_Agent_Server;

    /*enum State
    {
        sttIdle,
        //sttEarly,
        sttConfirmed
    }*/

public:
    SIP_Dialog(SIP_Message_Type type);
    ~SIP_Dialog();

    void set_dialog(const char *call_id, const char *local_tag, const char *remote_tag,
            unsigned int local_sequence_number, unsigned int remote_sequence_number,
            const char *local_uri, const char *remote_uri, const char *remote_target);
    void set_remote_target(const char *remote_target);
    void add_route_back(SIP_Header_Route *route);
    void add_route_front(SIP_Header_Route *route);
    void clear_routes();

    int get_routes_size() { return (int) _routes.size(); }
    SIP_Header_Route *get_route(int pos);

private:
    SIP_Message_Type _type;

    //State _state;

    char *_call_id;
    char *_local_tag;
    char *_remote_tag;
    unsigned int _local_sequence_number;
    unsigned int _remote_sequence_number;
    char *_local_uri;
    char *_remote_uri;
    char *_remote_target;
    Simple_List<SIP_Header/*Route*/> _routes;

    Simple_List<SIP_Dialog>::Element _link;
};


class SIP_Subscription
{
    friend class SIP_User_Agent;
    friend class SIP_User_Agent_Client;
    friend class SIP_User_Agent_Server;

private:
    SIP_Event_Package _event_type;
    char *_event_id;

    //Functor_Handler *_timer_handler;
    //Alarm *_timer_alarm;

public:
    SIP_Subscription() : _event_type(SIP_EVENT_PACKAGE_INVALID), _event_id(0)/*, _timer_handler(0), _timer_alarm(0)*/ {}
    ~SIP_Subscription() { clear(); }

    void set_subscription(SIP_Event_Package event_type, const char *event_id);
    //void start_timer(void *p);
    //void stop_timer();

    bool is_active() { return _event_type != SIP_EVENT_PACKAGE_INVALID; }
    void clear();
};


class SIP_User_Agent_Client
{
    friend class SIP_User_Agent;

public:
    SIP_User_Agent_Client(SIP_User_Agent *ua) : _ua(ua) {}
    ~SIP_User_Agent_Client() {}

private:
    SIP_Request *create_request(SIP_Message_Type msg_type, SIP_Dialog *dialog = 0, const char *to = 0, SIP_Message *invite = 0);

public:
    SIP_Request_Ack *create_ack(const char *to, SIP_Request_Invite *invite);
    SIP_Request_Bye *create_bye(const char *to);
    SIP_Request_Invite *create_invite(const char *to);
    SIP_Request_Message *create_message(const char *to, const char *data);
    SIP_Request_Notify *create_notify(const char *to, SIP_Subscription_State state, SIP_Pidf_Xml_Basic_Element pidf_xml_element, unsigned int expires = 0);
    SIP_Request_Subscribe *create_subscribe(const char *to) { return 0; }
    void send_request(SIP_Request *request);

    bool receive_msg(SIP_Response *response);
    bool receive_msg(SIP_Request *request, SIP_Response *response, SIP_Transaction *transaction);
    //bool receive_1xx(SIP_Request *request, SIP_Response *response, SIP_Transaction *transaction, SIP_Dialog *dialog) { return true; }
    bool receive_2xx(SIP_Request *request, SIP_Response *response, SIP_Transaction *transaction, SIP_Dialog *dialog);
    bool receive_3xx_6xx(SIP_Request *request, SIP_Response *response, SIP_Transaction *transaction, SIP_Dialog *dialog);

    SIP_Dialog *create_dialog(SIP_Request *request, SIP_Response *response);

private:
    SIP_User_Agent *_ua;
};


class SIP_User_Agent_Server
{
    friend class SIP_User_Agent;

public:
    SIP_User_Agent_Server(SIP_User_Agent *ua) : _ua(ua) {}
    ~SIP_User_Agent_Server() {}

private:
    SIP_Response *create_response(int status_code, SIP_Request *request);
    void send_response(SIP_Response *response, SIP_Message_Type request_type, SIP_Transaction *transaction);

public:
    bool receive_msg(SIP_Request *request);
    bool receive_msg(SIP_Request *request, SIP_Transaction *transaction);
    //bool receive_ack(SIP_Request_Ack *request, SIP_Transaction *transaction, SIP_Dialog *dialog) { return true; }
    bool receive_bye(SIP_Request_Bye *request, SIP_Transaction *transaction, SIP_Dialog *dialog);
    bool receive_invite(SIP_Request_Invite *request, SIP_Transaction *transaction, SIP_Dialog *dialog);
    bool receive_message(SIP_Request_Message *request, SIP_Transaction *transaction, SIP_Dialog *dialog);
    //bool receive_notify(SIP_Request_Notify *request, SIP_Transaction *transaction, SIP_Dialog *dialog) { return true; }
    bool receive_subscribe(SIP_Request_Subscribe *request, SIP_Transaction *transaction, SIP_Dialog *dialog);

    SIP_Dialog *create_dialog(SIP_Request *request, SIP_Response *response);

private:
    SIP_User_Agent *_ua;
};


class SIP_User_Agent
{
    friend class SIP_User_Agent_Client;
    friend class SIP_User_Agent_Server;
    friend class SIP_Transaction_Client_Invite;
    friend class SIP_Transaction_Client_Non_Invite;
    friend class SIP_Transaction_Server_Invite;
    friend class SIP_Transaction_Server_Non_Invite;
    friend class SIP_Manager;

public:
    SIP_User_Agent(const char *uri);
    ~SIP_User_Agent();

    SIP_User_Agent_Client *get_uac() { return &_uac; }
    SIP_User_Agent_Server *get_uas() { return &_uas; }

    SIP_Dialog *matchingDialog(SIP_Message *msg, SIP_Message_Type type);
    SIP_Dialog *matchingDialog(const char *to, SIP_Message_Type type);
    SIP_Transaction *matching_transaction(SIP_Message *msg);

    const char *get_uri() { return _uri; }
    const char *get_text_received() { return _text_received; }
    bool has_subscription() { return _subscription.is_active(); }
    const char *get_subscriber();

    void add_transaction(SIP_Transaction *transaction) { _transactions.insert(&transaction->_link); }
    void remove_transaction(SIP_Transaction *transaction) { _transactions.remove(&transaction->_link); delete transaction; }
    SIP_Dialog *add_dialog(SIP_Message_Type type);
    void remove_dialog(SIP_Dialog *dialog) { _dialogs.remove(&dialog->_link); delete dialog; }

    int getTimerValue(SIP_Timer timer) { return _timer_values[timer]; }
    void setTimerValue(SIP_Timer timer, int timer_value) { _timer_values[timer] = timer_value; }
    void start_timer(SIP_Timer timer, SIP_Transaction *p);
    void stop_timer(SIP_Timer timer);

private:
    SIP_User_Agent_Client _uac;
    SIP_User_Agent_Server _uas;

    Simple_List<SIP_Dialog> _dialogs;
    SIP_Subscription _subscription;
    Simple_List<SIP_Transaction> _transactions;

    char *_uri;
    const char *_text_received;

    int _timer_values[SIP_TIMER_COUNT];
    Functor_Handler<SIP_Transaction> *_timer_handlers[SIP_TIMER_COUNT];
    Alarm *_timer_alarms[SIP_TIMER_COUNT];

    Simple_List<SIP_User_Agent>::Element _link;
};


class Send_RTP
{
public:
    Send_RTP() : _sequence(0x016a), _timestamp(0x00207a10), _udp(IP::instance()), _socket(&_udp) {}
    ~Send_RTP() {}

    void send_data(const char *destination, unsigned short port, char *data, unsigned int size);

private:
    class My_Socket : public UDP::Socket
    {
    public:
        My_Socket(UDP *udp) : UDP::Socket(udp, UDP::Address(IP::instance()->address(), 8000), UDP::Address(Traits<IP>::BROADCAST, 8000)) {}
        virtual ~My_Socket() {}

        void received(const UDP::Address &src, const char *data, unsigned int size)
        {
            db<Send_RTP::My_Socket>(INF) << "Send_RTP::My_Socket::received..\n";
        }
    };

private:
    unsigned int _sequence;
    unsigned int _timestamp;

    UDP _udp;
    My_Socket _socket;
};

__END_SYS

#endif
