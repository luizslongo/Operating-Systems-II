// EPOS IP Protocol Declarations

#ifndef __ip_h
#define __ip_h

#include <utility/string.h>
#include <utility/hash.h>
#include <cpu.h>
#include <nic.h>
#include <system.h>

__BEGIN_SYS

class IP: private NIC::Observer, public Data_Observed<NIC::Buffer>
{
private:
    // List to hold received Buffers
    typedef NIC::Buffer Buffer;
    typedef Buffer::List List;
    typedef List::Element Element;

public:
    typedef NIC::Address MAC_Address;

    class Address: public NIC_Common::Address<4>
    {
    private:
        typedef NIC_Common::Address<4> Base;

    public:
        Address() {}
        Address(unsigned char a0, unsigned char a1 = 0, unsigned char a2 = 0, unsigned char a3 = 0): Base(a0, a1, a2, a3) {}
        Address(const char * s): Base(s, '.') {}
        Address(unsigned long a) { *reinterpret_cast<unsigned long *>(this) = htonl(a); }

        operator unsigned long() { return ntohl(*reinterpret_cast<unsigned long *>(this)); }
        operator unsigned long() const { return ntohl(*reinterpret_cast<const unsigned long *>(this)); }

        bool net_match(const Address & m, const Address & a) const {
            return (static_cast<unsigned long>(*this) & static_cast<unsigned long>(m)) != static_cast<unsigned long>(a);
        }
    } __attribute__((packed, may_alias));

    // RFC 1700 Protocols
    typedef unsigned char Protocol;
    enum {
        ICMP    = 0x01,
        TCP     = 0x06,
        UDP     = 0x11,
        RDP     = 0x1b,
        TTP     = 0x54
    };


    typedef Data_Observer<Buffer> Observer;
    typedef Data_Observed<Buffer> Observed;


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
            _ihl(IHL),  _version(VER), _tos(TOS), _length(htons(size)), _id(htons(_next_id++)),
            _offset(0), _flags(0), _ttl(TTL), _protocol(prot), _checksum(0), _from(from), _to(to) {}

        unsigned short length() const { return ntohs(_length); }
        void length(unsigned short length) { _length = htons(length); }

        unsigned short id() const { return ntohs(_id); }

        // Offsets in bytes that are converted to 8-byte unities
        unsigned short offset() const { return (ntohs((_flags << 13) | _offset) & 0x1fff) << 3; }
        void offset(unsigned short off) {
            unsigned short tmp = htons((flags() << 13) | (off >> 3));
            _offset = tmp & 0x1fff;
            _flags  = tmp >> 13;
        }

        unsigned short flags() const { return ntohs((_flags << 13) | _offset) >> 13; }
        void flags(unsigned short flg) {
            unsigned short tmp = htons((flg << 13) | (offset() >> 3));
            _offset = tmp & 0x1fff;
            _flags  = tmp >> 13;
        }

        unsigned char ttl() { return _ttl; }

        const Protocol & protocol() const { return _protocol; }

        unsigned short checksum() const { return ntohs(_checksum); }

        void sum() { _checksum = 0; _checksum = htons(IP::checksum(reinterpret_cast<unsigned char *>(this), _ihl * 4)); }
        bool check() { return !IP::checksum(reinterpret_cast<unsigned char *>(this), _ihl * 4); }

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
        unsigned char   _ihl:4;         // IP Header Length (in 32-bit words)
        unsigned char   _version:4;     // IP Version
        unsigned char    _tos;          // Type Of Service (not used -> 0)
        unsigned short  _length;        // Length of datagram in bytes (header + data)
        unsigned short  _id;            // Datagram id
        unsigned short  _offset:13;     // Fragment offset (x 8 bytes)
        unsigned short  _flags:3;       // Flags (DF, MF)
        unsigned char   _ttl;           // Time To Live
        Protocol        _protocol;      // Payload Protocol (RFC 1700)
        unsigned short  _checksum;      // Header checksum (RFC 1071)
        Address         _from;          // Source IP address
        Address         _to;            // Destination IP address

        static unsigned short _next_id;
    } __attribute__((packed, may_alias));


    static const unsigned int MTU = 65535 - sizeof(Header);
    typedef unsigned char Data[MTU];


    class Packet: public Header
    {
    public:
        Packet() {}
        Packet(const Address & from, const Address & to, const Protocol & prot, unsigned int size)
        : Header(from, to, prot, size + sizeof(Header)) {}
        Packet(const Address & from, const Address & to, const Protocol & prot, const void * data, unsigned int size)
        : Header(from, to, prot, size + sizeof(Header)) {
            header()->sum();
            memcpy(_data, data, size > sizeof(Data) ? sizeof(Data) : size);
        }

        Header * header() { return this; }
        unsigned char * data() { return _data; }

        friend Debug & operator<<(Debug & db, const Packet & p) {
            db << "{head=" << reinterpret_cast<const Header &>(p) << ",data=" << p._data << "}";
            return db;
        }

    private:
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
            for(; e && to.net_match(e->object()->_genmask, e->object()->_destination); e = e->next())
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
        typedef Hash::Element Element;

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
    IP(NIC * nic);
    ~IP() {}

    const Address & address() { return _address; }
    const Address & broadcast() { return _broadcast; }
    const Address & gateway() { return _gateway; }
    const Address & netmask() { return _netmask; }
    
    NIC * nic() { return _nic; }

    int send(const Address & to, const Protocol & protocol, Buffer * buf);

    static const unsigned int mtu() { return MTU; }

    static unsigned short checksum(const void * data, unsigned int size);

private:
    void update(NIC::Observed * nic, int prot, Buffer * buf);

    static const MAC_Address & arp(NIC * nic, const Address & log) { return ARP::get(log); }

    static const Route * route(const Address & to) { return Route::get(to); }

protected:
    Address _address;
    Address _netmask;
    Address _broadcast;
    Address _gateway;

    NIC * _nic;

    static List _fragmented;
};

__END_SYS

#endif

