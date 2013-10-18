#ifndef __ip_h
#define __ip_h

#include <cpu.h>
#include <nic.h>
#include <utility/string.h>
#include <utility/hash.h>
#include <system.h>

__BEGIN_SYS

class IP// : public Traits<IP>, public Active, public Data_Observed<IP_Address>
{
protected:

    // Imports from CPU
    typedef CPU::Reg8  u8;
    typedef CPU::Reg16 u16;
    typedef CPU::Reg32 u32;

    static const unsigned int MTU = 65516;

public:
    typedef NIC::Address MAC_Address;

    typedef NIC_Common::Address<4> Address;
    typedef unsigned char Data[MTU];

    typedef u8 Protocol;
    enum {
        ICMP    = 0x01,
        TCP     = 0x06,
        UDP     = 0x11,
        RDP     = 0x1b,
        TTP     = 0x54
    };

//    typedef Data_Observer<Address> Observer;
//    typedef Data_Observed<Address> Observed;


    class Header
    {
    public:
        static const unsigned int VER = 4;
        static const unsigned int IHL = 5; // 20 bytes
        static const unsigned int TOS = 0;
        static const unsigned int TTL = Traits<IP>::TTL;

        enum {
            MF = 1, // More Fragments
            DF = 2  // Don't Fragment
        };

    public:
        Header() {}

        Header(const Address & from, const Address & to, const Protocol & prot, unsigned int size) :
            _ihl(IHL),  _version(VER), _tos(TOS), _length(CPU::htons(size)), _id(CPU::htons(_next_id++)),
            _offset(0), _flags(0), _ttl(TTL), _protocol(prot), _checksum(0), _from(from), _to(to) {
            _checksum = ~checksum(reinterpret_cast<const unsigned char *>(this), hlength());
        }

        u16 hlength() { return _ihl * 4; }

        u16 length() const { return CPU::ntohs(_length); }
        void length(u16 length) { _length = CPU::htons(length); }

        u16 id() const { return CPU::ntohs(_id); }

        u16 offset() const { return CPU::ntohs((_flags << 13) | _offset) & 0x1fff; }
        void offset(u16 off) {
            u16 tmp = CPU::htons((flags() << 13) | off);
            _offset = tmp & 0x1fff;
            _flags  = tmp >> 13;
        }

        u16 flags() const { return CPU::ntohs((_flags << 13) | _offset) >> 13; }
        void flags(u16 flg) {
            u16 tmp = CPU::htons((flg << 13) | offset());
            _offset = tmp & 0x1fff;
            _flags  = tmp >> 13;
        }

        u8 ttl() { return _ttl; }
        void ttl(u8 ttl) { _ttl = ttl; }

        const Protocol & protocol() const { return _protocol; }
        void protocol(const Protocol & protocol) { _protocol = protocol; }

        bool check();

        const Address & from() const { return _from; }
        void from(const Address & from){ _from = from; }
        const Address & to() const { return _to; }
        void to(const Address & to){ _to = to; }

        friend Debug & operator<<(Debug & db, const Header & h) {
            db << "{ver=" << h._version
               << ",ihl=" << h._ihl
               << ",tos=" << h._tos
               << ",len=" << h.length()
               << ",id="  << h.id()
               << ",off=" << h.offset()
               << ",flg=" << ((h.flags() & DF) ? "DF" : (h.flags() & MF) ? "MF" : "--")
               << ",ttl=" << h._ttl
               << ",pro=" << h._protocol
               << ",chk=" << hex << h._checksum << dec
               << ",from=" << h._from
               << ",to=" << h._to
               << "}";
            return db;
        }

    private:
        u8  _ihl:4;             // IP Header Length (in 32-bit words)
        u8  _version:4;         // IP Version
        u8  _tos;               // Type Of Service (not used -> 0)
        u16 _length;            // Size of datagram (header + data)
        u16 _id;                // Datagram id
        u16 _offset:13;         // Fragment offset (x 8 bytes)
        u16 _flags:3;           // Flags (UN, DF, MF)
        u8  _ttl;               // Time To Live
        Protocol  _protocol;    // RFC 1700 (1->ICMP, 6->TCP, 17->UDP)
        volatile u16 _checksum; // Header checksum
        Address _from;           // Source IP address
        Address _to;           // Destination IP addres

        static u16 _next_id;
    } __attribute__((packed, may_alias));


    // Pseudo header for checksum calculations
    struct Pseudo_Header {
        u32 from_ip;
        u32 to_ip;
        u8 zero;
        u8 protocol;
        u16 length;
    };


    class Packet
    {
    public:
        Packet() {}

        Packet(const Address & from, const Address & to, const Protocol & prot, const void * data, unsigned int size)
        : _header(from, to, prot, size + sizeof(Header)) {
            memcpy(_data, data, size > sizeof(Data) ? sizeof(Data) : size);
        }

        Header & header() { return _header; }
        unsigned char * data() { return _data; }

        u16 length() const { return _header.length(); }
        const Protocol & protocol() const { return _header.protocol(); }
        const Address & from() const { return _header.from(); }
        const Address & to() const { return _header.to(); }

        friend Debug & operator<<(Debug & db, const Packet & p) {
            db << "{head=" << p._header << ",data=" << p._data << "}";
            return db;
        }

    private:
        Header _header;
        Data _data;
    } __attribute__((packed, may_alias));

    typedef Packet PDU;

    static const unsigned int MAX_FRAGMENT = sizeof(NIC::Data) - sizeof(Header);

    class Route
    {
    private:
        typedef Simple_List<Route> Table;
        typedef Table::Element Element;

    public:
        Route(NIC * n, const Address & d, const Address & g, const Address & m, unsigned int f = 0, unsigned int w = 0)
        : _destination(d), _gateway(g), _genmask(m), _flags(f), _metric(w), _nic(n), _link(this) {
            _table.insert(&_link);
            db<IP>(INF) << "IP::Route(this=" << this << ") => " << *this << endl;
        }
        ~Route() { _table.remove(&_link); }

        const Address & gateway() const { return _gateway; }

        NIC * nic() const { return _nic; }

        static const Route * get(const Address & to) { // Assume default (0.0.0.0) to have genmask 0.0.0.0 and be the last route in table
            Element * e = _table.head();
            for(; e && (static_cast<unsigned long>(to) & static_cast<unsigned long>(e->object()->_genmask)) != static_cast<unsigned long>(e->object()->_destination); e = e->next())
                db<IP>(INF) << "IP::Route::search([route=" << e->object() << " => " << *e->object() << endl;
            return e->object();
        }

        friend Debug & operator<<(Debug & db, const Route & r) {
            db << "{d=" << r._destination
               << ",g=" << r._gateway
               << ",m=" << r._genmask
               << ",f=" << r._flags
               << ",w=" << r._metric
               << ",nic=" << r._nic
               << "}";
            return db;
        }

    private:
        Address _destination;
        Address _gateway;
        Address _genmask;
        unsigned int _flags;
        unsigned int _metric;
        NIC * _nic;
        Element _link;

        static Table _table;
    };

    class ARP
    {
    private:
        typedef Simple_Hash<ARP, 10, Address> Hash;
        typedef Simple_Hash<ARP, 10, Address>::Element Element;

    public:
        ARP(NIC * n, const Address & log, const MAC_Address & phy)
        : _log(log), _phy(phy), _nic(n), _link(this, log) {
            _table.insert(&_link);
        }
        ~ARP() { _table.remove(&_link); }

        static const MAC_Address & get(const Address & log) { return _table.search_key(log)->object()->_phy; }

    private:
        Address _log;
        MAC_Address _phy;
        NIC * _nic;
        Element _link;

        static Hash _table;
    };

public:
    IP();
    ~IP();

    const Address & address() { return _address; }
    const Address & broadcast() { return _broadcast; }
    const Address & gateway() { return _gateway; }
    const Address & netmask() { return _netmask; }
    
    int send(const Address & to, const Protocol & prot, const void * data, unsigned int size);
    int receive(Address * from, Protocol * prot, void * data, unsigned int size);

    static const unsigned int mtu() { return MTU; }

    NIC * nic() { return _nic; }

private:
    IP(const Address & a, const Address & m, const Address & b, const Address & g)
    : _address(a), _netmask(m), _broadcast(b), _gateway(g) {
        new (SYSTEM) Route(_nic, (a & m), a, m);
        new (SYSTEM) Route(_nic, 0UL, g, 0UL);

        new (SYSTEM) ARP(_nic, a, _nic->BROADCAST);
        new (SYSTEM) ARP(_nic, g, _nic->BROADCAST);
    }

    bool is_local(const Address & address) const
    {
        return (_address & _netmask) == (u32(address) & u32(_netmask));
    }

    static const MAC_Address & arp(const Address & log) { return ARP::get(log); }

    const Route * route(const Address & to) { return Route::get(to); }

//  void update(NIC::Observed * o, int p);

    // From http://www.faqs.org/rfcs/rfc1071.html
    static u16 checksum(const unsigned char * data, u16 size) {
        u32 sum = 0;

        for(u16 i = 0; i < size - 1; i += 2)
            sum += (((unsigned short)(data[i + 1]) & 0x00FF) << 8) | data[i];

        if(size & 1)
            sum += data[size-1];

        while(sum >> 16)
            sum = (sum & 0xffff) + (sum >> 16);

        return sum;
    }

protected:
    Address _address;
    Address _netmask;
    Address _broadcast;
    Address _gateway;

    static NIC * _nic;
};

__END_SYS

#endif

