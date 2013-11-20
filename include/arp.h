// EPOS ARP Protocol Declarations

#ifndef __arp_h
#define __arp_h

#include <utility/hash.h>
#include <alarm.h>
#include <condition.h>

__BEGIN_SYS

template<typename NIC, typename Network, unsigned int HTYPE = 1>  // Ethernet Hardware Type (HTYPE) = 1
class ARP: private NIC::Observer
{
private:
    static const unsigned int ENTRIES = Traits<Network>::NODES;

    class Entry;
    typedef Simple_Hash<Entry, ENTRIES> Hash;
    typedef typename Hash::Element Element;


    // Addresses
    typedef typename Network::Address PA;
    typedef typename NIC::Address HA;


    // ARP Table Entry
     class Entry
     {
     public:
         Entry(const PA & pa, const HA & ha = HA::BROADCAST): _ha(ha), _link(this, pa) {}

         HA _ha;
         Condition _condition;
         Element _link;
     };


//     class ARP
//     {
//     private:
//         typedef Simple_Hash<ARP, 10, Address> Hash;
//         typedef Hash::Element Element;
//
//     public:
//         ARP(NIC * n, const Address & log, const MAC_Address & phy)
//         : _log(log), _phy(phy), _nic(n), _link(this, log) {
//             _table.insert(&_link);
//         }
//         ~ARP() { _table.remove(&_link); }
//
//         static const MAC_Address & get(const Address & log) { return _table.search_key(log)->object()->_phy; }
//
//     private:
//         Address _log;
//         MAC_Address _phy;
//         NIC * _nic;
//         Element _link;
//
//         static Hash _table;
//     };


     // ARP Table
     class Table: private Hash
     {
     public:
         Condition * insert(const PA & pa) {
             Element * el = Hash::search_key(pa);
             Entry * e;

             if(el)
                 e = el->object();
             else {
                 e = new (SYSTEM) Entry(pa);
                 Hash::insert(&e->_link);
             }

             return &e->_condition;
         }

         void update(const PA & pa, const HA & ha) {
             Element * el = Hash::search_key(pa);
             Entry * e;

             if(el) {
                 e = el->object();
                 e->_ha = ha;
                 e->_condition.broadcast();
             } else {
                 e = new (SYSTEM) Entry(pa, ha);
                 Hash::insert(&e->_link);
             }
         }

         HA search(const PA & pa) {
             Element * el = Hash::search_key(pa);
             return el ? el->object()->_ha : HA::NULL;
         }
     };


public:
    // ARP/RARP Operations
    typedef unsigned short Oper;
    enum {
        REQUEST      = 1,
        REPLY        = 2,
        RARP_REQUEST = 3,
        RARP_REPLY   = 4
    };


    // ARP Packet
    class Packet
    {
    public:
        Packet(Oper op, const HA & sha, const PA & spa, const HA & tha, const PA & tpa)
        : _htype(htons(HTYPE)), _ptype(htons(Network::PROTOCOL)), _hlen(sizeof(HA)), _plen(sizeof(PA)),
          _oper(htons(op)), _sha(sha), _spa(spa), _tha(tha), _tpa(tpa) {}
        ~Packet() {}

        void op(const Oper & o) { _oper = htons(o); }
        void sha(const HA & a) { _sha = a; }
        void spa(const PA & a) { _spa = a; }
        void tha(const HA & a) { _tha = a; }
        void tpa(const PA & a) { _tpa = a; }

        Oper op() const { return ntohs(_oper); }
        const HA & sha() const { return _sha; }
        const PA & spa() const { return _spa; }
        const HA & tha() const { return _tha; }
        const PA & tpa() const { return _tpa; }

        friend Debug & operator<<(Debug & db, const Packet & p)
        {
            db  << "{htp=" << ntohs(p._htype)
                << ",ptp=" << hex << ntohs(p._ptype) << dec
                << ",hln=" << p._hlen
                << ",pln=" << p._plen
                << ",opc=" << ntohs(p._oper)
                << ",sha=" << p._sha
                << ",spa=" << p._spa
                << ",tha=" << p._tha
                << ",tpa=" << p._tpa << "}";
            return db;
        }

    private:
        unsigned short  _htype; // Hardware Type
        unsigned short  _ptype; // Protocol Type
        unsigned char   _hlen;  // Hardware Address Length
        unsigned char   _plen;  // Protocol Address Length
        unsigned short  _oper;  // Operation
        HA              _sha;   // Sender Hardware Address (48 bits)
        PA              _spa;   // Sender Protocol Address (32 bits)
        HA              _tha;   // Target Hardware Address (48 bits)
        PA              _tpa;   // Target Protocol Address (32 bits)
    } __attribute__((packed, may_alias));


public:
    ARP(NIC * nic, Network * net): _nic(nic), _net(net) { _nic->attach(this, NIC::ARP); }

    void update(const PA & pa, const HA & ha) { _table.update(pa, ha); }

    const HA & resolve(const PA & pa) {
        db<ARP>(TRC) << "ARP::resolve(pa=" << pa << ")" << endl;

        const HA & ha = _table.search(pa);
        for(unsigned int i = 0; (i < Traits<Network>::RETRIES) && !ha; i++) {
            Condition * cond = _table.insert(pa);

            Packet request(REQUEST, _nic->address(), _net->address(), HA::BROADCAST, pa);

            db<ARP>(INF) << "ARP::resolve:packet=" << &request << " => " << request << endl;

            _nic->send(HA::BROADCAST, NIC::ARP, &request, sizeof(Packet));

            db<ARP>(INF) << "ARP::resolve:request sent!" << endl;

            Condition_Handler handler(cond);
            Alarm alarm(Traits<Network>::TIMEOUT * 1000000, &handler, 1);
            cond->wait();
        }

        db<ARP>(INF) << "ARP::resolve(pa=" << pa << ") => " << ha << endl;

        return ha;
    }

    const PA & resolve(const HA & ha) {
        db<ARP>(TRC) << "ARP::resolve(ha=" << ha << ")" << endl;

        PA pa = PA(0);
        for(unsigned int i = 0; (i < Traits<Network>::RETRIES) && !pa; i++) {
            Condition * cond = _table.insert(pa);

            Packet request(RARP_REQUEST, ha, pa, ha, pa);

            _nic->send(HA::BROADCAST, NIC::RARP, &request, sizeof(Packet));

            db<ARP>(INF) << "RARP::resolve:request sent => " << request << endl;

            Condition_Handler handler(cond);
            Alarm alarm(Traits<Network>::TIMEOUT * 1000000, &handler, 1);
            cond->wait();
        }

        db<Network>(TRC) << "RARP::resolve(ha=" << ha << ") => " << pa << endl;

        return pa;
    }

    void update(typename NIC::Observed * nic, int prot, typename NIC::Buffer * buf)
    {
        db<ARP>(TRC) << "ARP::update(nic=" << nic << ",prot=" << prot << ",buf=" << buf << ")" << endl;

        Packet * packet = buf->frame()->template data<Packet>();

        db<ARP>(INF) << "ARP::update:pkt=" << packet << " => " << *packet << endl;

        if((packet->op() == REQUEST) && (packet->tpa() == _net->address())) {
            Packet reply(REPLY, _nic->address(), _net->address(), packet->sha(), packet->spa());

            db<ARP>(INF) << "ARP::update:reply sent => " << reply << endl;

            _nic->send(packet->sha(), NIC::ARP, &reply, sizeof(Packet));
        } else if((packet->op() == REPLY) && (packet->tha() == _nic->address())) {
            db<ARP>(INF) << "ARP::update:reply received => " << *packet << endl;

            _table.update(packet->spa(), packet->sha());
        } else
            db<ARP>(WRN) << "ARP::update:unknown packet type (" << prot << ")" << endl;
    }

private:
    Table _table;
    NIC * _nic;
    Network * _net;
};

__END_SYS

#endif
