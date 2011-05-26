#ifndef __sip_transactions_h
#define __sip_transactions_h

#include <sip_defs.h>
#include <sip_manager.h>
#include <sip_message.h>

__BEGIN_SYS

class SIP_User_Agent_Client;
class SIP_User_Agent_Server;

class SIP_Transaction
{
    friend class SIP_User_Agent;

public:
    SIP_Transaction(SIP_User_Agent *ua);
    virtual ~SIP_Transaction() {}

    virtual SIP_Transaction_Type get_transaction_type() = 0;
    virtual SIP_Transaction *matching_transaction(SIP_Message *msg) = 0;
    void clear();

protected:
    SIP_Transport_Layer *_transport;
    SIP_User_Agent *_ua;

    Simple_List<SIP_Transaction>::Element _link;
};


class SIP_Transaction_Client_Invite : public SIP_Transaction
{
private:
    enum State
    {
        sttIdle,
        sttCalling,
        sttProceeding,
        sttCompleted,
        //sttTerminated
    };

public:
    SIP_Transaction_Client_Invite(SIP_User_Agent *ua);
    ~SIP_Transaction_Client_Invite();

    void send_ack(SIP_Response *msg);

    void send_invite(SIP_Request_Invite *msg);
    void receive_1xx(SIP_Response *msg);
    void receive_2xx(SIP_Response *msg);
    void receive_3xx_6xx(SIP_Response *msg);
    void transport_error();

    void timer_A_Expired();
    void timer_B_Expired();
    void timer_D_Expired();

    static void timer_A_Callback(SIP_Transaction *p) { ((SIP_Transaction_Client_Invite *) p)->timer_A_Expired(); }
    static void timer_B_Callback(SIP_Transaction *p) { ((SIP_Transaction_Client_Invite *) p)->timer_B_Expired(); }
    static void timer_D_Callback(SIP_Transaction *p) { ((SIP_Transaction_Client_Invite *) p)->timer_D_Expired(); }

    SIP_Transaction_Type get_transaction_type() { return SIP_TRANSACTION_CLIENT_INVITE; }
    SIP_Transaction *matching_transaction(SIP_Message *msg);

private:
    State _state;
    SIP_Request_Invite *_request_msg;
};


class SIP_Transaction_Client_Non_Invite : public SIP_Transaction
{
private:
    enum State
    {
        sttIdle,
        sttTrying,
        sttProceeding,
        sttCompleted,
        //sttTerminated
    };

public:
    SIP_Transaction_Client_Non_Invite(SIP_User_Agent *ua);
    ~SIP_Transaction_Client_Non_Invite();

    //void copy_request(SIP_Request *msg);

    void send_request(SIP_Request *msg);
    void receive_1xx(SIP_Response *msg);
    void receive_2xx_6xx(SIP_Response *msg);
    void transport_error();

    void timer_E_Expired();
    void timer_F_Expired();
    void timer_K_Expired();

    static void timer_E_Callback(SIP_Transaction *p) { ((SIP_Transaction_Client_Non_Invite *) p)->timer_E_Expired(); }
    static void timer_F_Callback(SIP_Transaction *p) { ((SIP_Transaction_Client_Non_Invite *) p)->timer_F_Expired(); }
    static void timer_K_Callback(SIP_Transaction *p) { ((SIP_Transaction_Client_Non_Invite *) p)->timer_K_Expired(); }

    SIP_Transaction_Type get_transaction_type() { return SIP_TRANSACTION_CLIENT_NON_INVITE; }
    SIP_Transaction *matching_transaction(SIP_Message *msg);

private:
    State _state;
    SIP_Request *_request_msg;
};


class SIP_Transaction_Server_Invite : public SIP_Transaction
{
private:
    enum State
    {
        sttIdle,
        sttProceeding,
        sttCompleted,
        sttConfirmed,
        //sttTerminated
    };

public:
    SIP_Transaction_Server_Invite(SIP_User_Agent *ua);
    ~SIP_Transaction_Server_Invite();

    void receive_invite(SIP_Request_Invite *msg);
    void receive_ack(SIP_Request_Ack *msg);
    void send_1xx(SIP_Response *msg);
    void send_2xx(SIP_Response *msg);
    void send_3xx_6xx(SIP_Response *msg);
    void transport_error();

    void timer_G_Expired();
    void timer_H_Expired();
    void timer_I_Expired();

    static void timer_G_Callback(SIP_Transaction *p) { ((SIP_Transaction_Server_Invite *) p)->timer_G_Expired(); }
    static void timer_H_Callback(SIP_Transaction *p) { ((SIP_Transaction_Server_Invite *) p)->timer_H_Expired(); }
    static void timer_I_Callback(SIP_Transaction *p) { ((SIP_Transaction_Server_Invite *) p)->timer_I_Expired(); }

    SIP_Transaction_Type get_transaction_type() { return SIP_TRANSACTION_SERVER_INVITE; }
    SIP_Transaction *matching_transaction(SIP_Message *msg);

private:
    State _state;
    SIP_Request_Invite *_request_msg;
    SIP_Response *_last_response;
};


class SIP_Transaction_Server_Non_Invite : public SIP_Transaction
{
private:
    enum State
    {
        sttIdle,
        sttTrying,
        sttProceeding,
        sttCompleted,
        //sttTerminated
    };

public:
    SIP_Transaction_Server_Non_Invite(SIP_User_Agent *ua);
    ~SIP_Transaction_Server_Non_Invite();

    //void copy_request(SIP_Request *msg);

    void receive_request(SIP_Request *msg);
    void send_1xx(SIP_Response *msg);
    void send_2xx_6xx(SIP_Response *msg);
    void transport_error();

    void timer_J_Expired();

    static void timer_J_Callback(SIP_Transaction *p) { ((SIP_Transaction_Server_Non_Invite *) p)->timer_J_Expired(); }

    SIP_Transaction_Type get_transaction_type() { return SIP_TRANSACTION_SERVER_NON_INVITE; }
    SIP_Transaction *matching_transaction(SIP_Message *msg);

private:
    State _state;
    SIP_Request *_request_msg;
    SIP_Response *_last_response;
};

__END_SYS

#endif
