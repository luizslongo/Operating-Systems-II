// EPOS Communicator Declarations

#ifndef __communicator_h
#define __communicator_h

#include <network.h>
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
    Communicator_Common(): _ready(0) {}

public:
    ~Communicator_Common() {}

    int send(const Local_Address & from, const Address & to, const void * data, unsigned int size) {
        return _channel.send(from, to, data, size);
    }
    int receive(void * data, unsigned int size) {
        _ready.p();
        Element * el = _received.remove();
        Buffer * buf = el->object();
        delete el;
        return _channel.receive(buf, data, size);
    }

    void bind(const Local_Address & port) {
        _channel.attach(this, port);
    }
    void unbind(const Local_Address & port) {
        _channel.detach(this, port);
    }

private:
    void update(typename Channel::Observed * channel, int local, Buffer * buf) {
        _received.insert(new (SYSTEM) Element(buf));
        _ready.v();
    }

private:
    Semaphore _ready;
    List _received;

    Channel _channel;
};

template<typename Channel, typename Network = typename Channel::Network>
class Link: public Communicator_Common<Channel, Network>
{
private:
    typedef Communicator_Common<Channel, Network> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Link(const Local_Address & from, const Address & to): _from(from), _to(to) { bind(from); }
    ~Link() { unbind(_from); }

    int send(const void * data, unsigned int size) { return Base::send(_from, _to, data, size); }
    int receive(void * data, unsigned int size) { return Base::receive(data, size); }

    const Address & peer() const { return _to;}

private:
    Local_Address _from;
    Address _to;
};


template<typename Channel, typename Network = typename Channel::Network>
class Port: private Communicator_Common<Channel, Network>
{
private:
    typedef Communicator_Common<Channel, Network> Base;

public:
    // Channel imports
    typedef typename Channel::Address Address;
    typedef typename Channel::Address::Local Local_Address;

public:
    Port(Network * network, const Local_Address & port): Base(network), _from(network->address(), port) {
        bind(port);
    }
    ~Port() {
        unbind(_from.local());
    }

    int send(const Address & to, const void * data, unsigned int size) {
        return Base::send(_from, to, data, size);
    }
    int receive(void * data, unsigned int size) {
        return Base::receive(data, size);
    }

private:
    Address _from;
};

__END_SYS

#endif
