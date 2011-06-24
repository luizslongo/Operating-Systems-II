#ifndef __ieee1451_tim_h
#define __ieee1451_tim_h

#include <ieee1451_objects.h>
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
    void send_msg(unsigned short trans_id, const char *message, unsigned int length);
    void receive_msg(unsigned short trans_id, const char *message, unsigned int size);

    void set_transducer(IEEE1451_Transducer *transducer) { _transducer = transducer; }

private:
    class TIM_Socket : public TCP::ClientSocket
    {
    public:
        TIM_Socket(const IP::Address &dst) : TCP::ClientSocket(TCP::Address(dst, IEEE1451_PORT),
            TCP::Address(IP::instance()->address(), IEEE1451_PORT)), _data(0), _length(0) {}
        ~TIM_Socket() { if (_data) delete _data; }

        void send(const char *data, unsigned int length);

        void connected();
        void closed();
        void received(const char *data, u16 size);
        void error(short error);
        void sent(u16 size);

    private:
        const char *_data;
        unsigned int _length;
    };

private:
    TIM_Socket *_socket;

    bool _connected;
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
