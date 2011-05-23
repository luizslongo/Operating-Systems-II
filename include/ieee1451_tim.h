#ifndef __ieee1451_tim_h
#define __ieee1451_tim_h

#include <ieee1451_objects.h>
#include <thread.h>
#include <ip.h>
#include <tcp.h>

__BEGIN_SYS

class IEEE1451_TEDS_TIM
{
public:
    IEEE1451_TEDS_TIM(char *payload, unsigned short size)
        : _payload(payload), _size(size) {}

    const char *get_payload() { return _payload; }
    unsigned int get_size() { return _size; }

private:
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


class IEEE1451_Dot0_TIM
{
private:
    IEEE1451_Dot0_TIM();

public:
    ~IEEE1451_Dot0_TIM();

    static IEEE1451_Dot0_TIM *get_instance();

    void init_teds();
    IEEE1451_TEDS_TIM *get_teds(char id);
    void receive_msg(unsigned short trans_id, const char *message, unsigned int size);

    void setTransducer(IEEE1451_Transducer *transducer) { _transducer = transducer; }

private:
    char *_meta_array;
    char *_tim_utn_array;
    char *_phy_array;
    IEEE1451_TEDS_TIM *_meta_teds;
    IEEE1451_TEDS_TIM *_tim_utn_teds;
    IEEE1451_TEDS_TIM *_phy_teds;

    IEEE1451_Transducer *_transducer;

    static IEEE1451_Dot0_TIM *_dot0;
};


class IEEE1451_Dot5_TIM
{
private:
    IEEE1451_Dot5_TIM();

public:
    ~IEEE1451_Dot5_TIM() {
        if (_socket)
            delete _socket;
    }

    static IEEE1451_Dot5_TIM *get_instance();

    void set_ncap_address(const IP::Address &addr) { _ncap_address = addr; }

    //bool connected() { return _connected; }
    void connect();
    void disconnect();
    void send_msg(unsigned short trans_id, const char *message, unsigned int length);

private:
    class My_Client_Socket : public TCP::ClientSocket
    {
    public:
        My_Client_Socket(TCP *tcp, const IP::Address &dst) :
            TCP::ClientSocket(tcp, TCP::Address(dst, IEEE1451_PORT),
            TCP::Address(IP::instance()->address(), IEEE1451_PORT)) {}
        ~My_Client_Socket() {}

        void connected();
        void closed();
        void received(const char *data, u16 size);
        void closing() {};
        void sent(u16 size) {};
        void error(short errorCode) {};
    };

private:
    TCP _tcp;
    My_Client_Socket *_socket;

    bool _connected;
    IP::Address _ncap_address;

    static IEEE1451_Dot5_TIM *_dot5;
};

__END_SYS

#endif
