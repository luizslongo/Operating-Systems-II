#ifndef __ieee1451_ncap_h
#define __ieee1451_ncap_h

#include <ieee1451_objects.h>
#include <thread.h>
#include <ip.h>
#include <tcp.h>
#include <utility/list.h>
#include <utility/malloc.h>

__BEGIN_SYS

class IEEE1451_Channel;
class IEEE1451_NCAP_Application;
class IEEE1451_TEDS_NCAP;

class IEEE1451_TLV
{
    friend class IEEE1451_TEDS_NCAP;

public:
    IEEE1451_TLV(char type, unsigned short length, char *value) :
        _type(type), _length(length), _value(value), _link(this) {}
    ~IEEE1451_TLV() { delete _value; }

    char get_type() { return _type; }
    unsigned short get_length() { return _length; }
    const char *get_value() { return _value; }

private:
    char _type;
    unsigned short _length;
    char *_value;

    Simple_List<IEEE1451_TLV>::Element _link;
};


class IEEE1451_TEDS_NCAP
{
    friend class IEEE1451_Channel;

public:
    IEEE1451_TEDS_NCAP(char id, const char *teds, unsigned short length, bool sub_block = false);
    ~IEEE1451_TEDS_NCAP();

    IEEE1451_TLV *get_tlv(char type);

private:
    char _id;
    Simple_List<IEEE1451_TLV> _tlvs;
    Simple_List<IEEE1451_TEDS_NCAP>::Element _link;
};


class IEEE1451_Channel
{
    friend class IEEE1451_NCAP_Application;

public:
    IEEE1451_Channel(const IP::Address &address) : _address(address) {}
    virtual ~IEEE1451_Channel();

    //virtual unsigned short get_channel_number() = 0;

    IP::Address get_address() { return _address; }
    IEEE1451_TEDS_NCAP *get_teds(char id);
    void add_teds(IEEE1451_TEDS_NCAP *teds) { _teds.insert(&teds->_link); }

private:
    IP::Address _address;
    Simple_List<IEEE1451_TEDS_NCAP> _teds;
};


class IEEE1451_Transducer_Channel : public IEEE1451_Channel
{
public:
    IEEE1451_Transducer_Channel(const IP::Address &address, unsigned short channel_number) :
        IEEE1451_Channel(address), _channel_number(channel_number) {}
    ~IEEE1451_Transducer_Channel() {}

    unsigned short get_channel_number() { return _channel_number; }

private:
    unsigned short _channel_number;
};


class IEEE1451_TIM_Channel : public IEEE1451_Channel
{
public:
    IEEE1451_TIM_Channel(const IP::Address &address, IEEE1451_Transducer_Channel *transducer) :
        IEEE1451_Channel(address), _transducer(transducer), _connected(false) {}
    ~IEEE1451_TIM_Channel() { delete _transducer; }

    IEEE1451_Transducer_Channel *get_transducer() { return _transducer; }
    //unsigned short get_channel_number() { return _tim_channel_number; }
    void connect() { _connected = true; }
    void disconnect() { _connected = false; }
    bool connected() { return _connected; }
    bool disconnected() { return _connected; }

private:
    IEEE1451_Transducer_Channel *_transducer;
    bool _connected;

    //static const unsigned short _tim_channel_number;
};


class IEEE1451_Dot0_NCAP
{
private:
    IEEE1451_Dot0_NCAP();

public:
    ~IEEE1451_Dot0_NCAP() {};

    static IEEE1451_Dot0_NCAP *get_instance();

    char *create_command(unsigned short channel_number, unsigned short command, const char *args = 0, unsigned int length = 0);
    unsigned short send_command(const IP::Address &destination, const char *message, unsigned int length);

    void tim_connected(const IP::Address &address);
    void tim_disconnected(const IP::Address &address);
    void receive_msg(const IP::Address &address, unsigned short trans_id, const char *message, unsigned int length);
    //void error_on_send(int error_code, unsigned short trans_id);

    void set_application(IEEE1451_NCAP_Application *application) { _application = application; }

private:
    IEEE1451_NCAP_Application *_application;

    static IEEE1451_Dot0_NCAP *_dot0;
};


class IEEE1451_Dot5_NCAP
{
private:
    class My_Server_Socket;

    IEEE1451_Dot5_NCAP();

public:
    ~IEEE1451_Dot5_NCAP();

    static IEEE1451_Dot5_NCAP *get_instance();

    My_Server_Socket *get_socket(const IP::Address &addr);
    void send_msg(unsigned short trans_id, const IP::Address &destination, const char *message, unsigned int length);

private:
    class My_Server_Socket : public TCP::ServerSocket
    {
    private:
        friend class IEEE1451_Dot5_NCAP;

        Simple_List<My_Server_Socket>::Element _link;

    public:
        My_Server_Socket(TCP *tcp) :
            TCP::ServerSocket(tcp, TCP::Address(IP::instance()->address(), IEEE1451_PORT)), _link(this) {};
        My_Server_Socket(const My_Server_Socket &socket) : TCP::ServerSocket(socket), _link(this) {};
        ~My_Server_Socket() {};

        TCP::Socket *incoming(const TCP::Address &from);
        void connected();
        void closed();
        void received(const char *data, u16 size);
        void closing();
        void sent(u16 size) {};
        void error(short errorCode) {};
    };

private:
    TCP _tcp;
    Simple_List<My_Server_Socket> _sockets;

    static IEEE1451_Dot5_NCAP *_dot5;
};

__END_SYS

#endif
