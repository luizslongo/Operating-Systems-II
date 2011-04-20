#ifndef __router_h
#define __router_h

#include <arp.h>
#include <utility/debug.h>

__BEGIN_SYS

/// A dummy router that always resolves logical address to broadcast addressess
template <class LinkLayer, class NetworkLayer>
class BCast_Router {
    typedef typename LinkLayer::Address    MAC_Address;
    typedef typename NetworkLayer::Address Address;
    typedef typename LinkLayer::Protocol   Protocol;
    typedef typename LinkLayer::PDU        PDU;
public:

    BCast_Router(LinkLayer * nic, NetworkLayer * network) { }

    void update(const Address & addr, const MAC_Address & mac)
    
    {
        // do nothing
    }
    
    MAC_Address resolve(const Address& addr)
    {
        return MAC_Address(LinkLayer::BROADCAST);    
    }
    
    void received(const MAC_Address& src, Protocol proto,
                  const PDU& data, int size) {}

   
};

/// The ARP_Router will resolve address using the ARP protocol
template <class LinkLayer,class NetworkLayer>
class ARP_Router {
protected:
    typedef typename LinkLayer::Address    MAC_Address;
    typedef typename NetworkLayer::Address Address;
    typedef typename LinkLayer::Protocol   Protocol;
    typedef typename LinkLayer::PDU        PDU;
    typedef ARP< LinkLayer , NetworkLayer >  _ARP;
    typedef typename _ARP::Packet          Packet;
        
    typename ARP<LinkLayer, NetworkLayer>::Table    _arpt;
    LinkLayer    * _nic;
    NetworkLayer * _net;
    
public:
    ARP_Router(LinkLayer * nic, NetworkLayer * net) : _nic(nic), _net(net)
    {
        
    }
    
    MAC_Address resolve(Address addr)
    {
        for(unsigned int i = 0; i < Traits<Network>::ARP_TRIES; i++)
        {
            MAC_Address pa = _arpt.search(addr);
            if(pa) {
                db<NetworkLayer>(TRC) << "ARP_Router::resolve(addr=" << addr << ") => "
                        << pa << "\n";

                return pa;
            }

            Condition * cond = _arpt.insert(addr);
            Packet request(_ARP::REQUEST, _nic->address(), _net->address(),
                                LinkLayer::BROADCAST, addr);
            
            _nic->send(LinkLayer::BROADCAST, LinkLayer::ARP, &request, sizeof(Packet));
        
            db<NetworkLayer>(INF) << "ARP_Router::resolve:request sent!\n";

            Condition_Handler handler(cond);
            //Alarm alarm(Traits<Network>::ARP_TIMEOUT, &handler, 1);
            Alarm alarm(100000, &handler, 1);
            cond->wait();
        }

        db<NetworkLayer>(TRC) << "ARP_Router::resolve(addr=" << addr << ") => not found!\n";

        return 0;
    }
    
    void update(Address addr, MAC_Address mac) {
        _arpt.update(addr, mac);
    }
    
    void received(const MAC_Address& src, Protocol proto,
                  const PDU& data, int size)
    {
        if (proto == LinkLayer::ARP) {
            const Packet& packet = *reinterpret_cast<const Packet *>(data);
            db<NetworkLayer>(INF) << "IP::update:ARP_Packet=" << packet << "\n";

            if((packet.op() == _ARP::REQUEST) && (packet.tpa() == _net->address()))
            {
                Packet reply(_ARP::REPLY, _nic->address(), _net->address(),
                        packet.sha(), packet.spa());
                
                db<NetworkLayer>(INF) << "IP::update: ARP_Packet=" << reply << "\n";
                _nic->send(packet.sha(), LinkLayer::ARP, &reply, sizeof(Packet));

                db<NetworkLayer>(INF) << "IP::update: ARP request answered!\n";
            }
            else if((packet.op() == _ARP::REPLY) && (packet.tha() == _nic->address())) 
            {
                db<NetworkLayer>(INF) << "IP::update: ARP reply received!\n";

                _arpt.update(packet.spa(), packet.sha());
            }    
        }
    }
   
private:
    // disable default constructor
    ARP_Router();
    // disable copy constructor
    ARP_Router(const ARP_Router& r);
};

__END_SYS

#endif

