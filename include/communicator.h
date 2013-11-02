// EPOS Communicator Declarations

#ifndef __communicator_h
#define __communicator_h

#include <udp.h>

__BEGIN_SYS 

template<typename Channel, typename Network = typename Channel::Network>
class Communicator_Common: private Channel::Observer
{
private:
    // List to hold received Buffers
    typedef NIC::Buffer Buffer;
    typedef Buffer::List List;
    typedef List::Element Element;

    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

protected:
    Communicator_Common(Network * network): _ready(0), _channel(network) {}
    ~Communicator_Common() {}

    int send(const void * data, unsigned int size) {
        return _channel.send(_from, _to, data, size);
    }
    int receive(void * data, unsigned int size) {
        _ready.p();
        Buffer * buf = reinterpret_cast<Buffer *>(_received.remove());
        return _channel.receive(buf, data, size);
    }

private:
    void update(typename Channel::Observed * channel, int local, Buffer * buf) {
        _received.insert(buf->link());
        _ready.v();
    }

protected:
    Semaphore _ready;
    List _received;

    Channel _channel;
};

template<typename Channel, typename Network = typename Channel::Network>
class Link: private Communicator_Common<Channel, Network>
{
public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Link(Network * network, const Local_Address & from, const Address & to): Communicator_Common(network), _from(network->address(), from), _to(to) {
        _channel.attach(this, from);
    }
    ~Link() {
        _channel.detach(this, _from.local());
    }

    int send(const void * data, unsigned int size) {
        return _channel.send(_from, _to, data, size);
    }
    int receive(void * data, unsigned int size) {
        return Communicator_Common::receive(data, size);
    }

private:
    Address _from;
    Address _to;
};


template<typename Channel, typename Network = typename Channel::Network>
class Port: private Communicator_Common<Channel, Network>
{
public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Port(Network * network, const Local_Address & port): Communicator_Common(network), _from(network->address(), port) {
        _channel.attach(this, port);
    }
    ~Port() {
        _channel.detach(this, _from.local());
    }

    int send(const Address & to, const void * data, unsigned int size) {
        return _channel.send(_from, to, data, size);
    }
    int receive(void * data, unsigned int size) {
        return Communicator_Common::receive(data, size);
    }

private:
    Address _from;
};

__END_SYS

#endif
