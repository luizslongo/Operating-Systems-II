// EPOS ICMP Protocol Declarations

#ifndef __icmp_h
#define __icmp_h

#include <ip.h>

__BEGIN_SYS

class ICMP: private IP::Observer, public Data_Observed<NIC::Buffer>
{
private:
    // Imports from IP
    typedef IP::Address Address;

    // ICMP Packet Types
    typedef unsigned char Type;
    enum
    {
        ECHO_REPLY              = 0,
        UNREACHABLE             = 3,
        SOURCE_QUENCH           = 4,
        REDIRECT                = 5,
        ALTERNATE_ADDRESS       = 6,
        ECHO                    = 8,
        ROUTER_ADVERT           = 9,
        ROUTER_SOLIC            = 10,
        TIME_EXCEEDED           = 11,
        PARAMETER_PROBLEM       = 12,
        TIMESTAMP               = 13,
        TIMESTAMP_REPLY         = 14,
        INFO_REQUEST            = 15,
        INFO_REPLY              = 16,
        ADDRESS_MASK_REQ        = 17,
        ADDRESS_MASK_REP        = 18,
        TRACEROUTE              = 30,
        DGRAM_ERROR             = 31,
        MOBILE_HOST_REDIR       = 32,
        IPv6_WHERE_ARE_YOU      = 33,
        IPv6_I_AM_HERE          = 34,
        MOBILE_REG_REQ          = 35,
        MOBILE_REG_REP          = 36,
        DOMAIN_NAME_REQ         = 37,
        DOMAIN_NAME_REP         = 38,
        SKIP                    = 39
    };

    // ICMP Packet Codes
    typedef unsigned char Code;
    enum
    {
        NETWORK_UNREACHABLE     = 0,
        HOST_UNREACHABLE        = 1,
        PROTOCOL_UNREACHABLE    = 2,
        PORT_UNREACHABLE        = 3,
        FRAGMENTATION_NEEDED    = 4,
        ROUTE_FAILED            = 5,
        NETWORK_UNKNOWN         = 6,
        HOST_UNKNOWN            = 7,
        HOST_ISOLATED           = 8,
        NETWORK_PROHIBITED      = 9,
        HOST_PROHIBITED         = 10,
        NETWORK_TOS_UNREACH     = 11,
        HOST_TOS_UNREACH        = 12,
        ADMIN_PROHIBITED        = 13,
        PRECEDENCE_VIOLATION    = 14,
        PRECEDENCE_CUTOFF       = 15
    };

    class Header
    {
    public:
        Header(const Type & type, const Code & code, unsigned short id, unsigned short seq):
            _type(type), _code(code), _checksum(0), _id(htons(id)), _sequence(htons(seq)) {}

        Type & type() { return _type; }
        Code & code() { return _code; }
        unsigned short checksum() { return _checksum; }
        unsigned short id() { return htons(_id); }
        unsigned short sequence() { return htons(_sequence); }

    private:
        unsigned char  _type;
        unsigned char  _code;
        unsigned short _checksum;
        unsigned short _id;
        unsigned short _sequence;
    };

    class Packet
    {
    private:
        static const unsigned int DATA_SIZE = 56;

    public:
        Packet(const Type & type, const Code & code, unsigned short id, unsigned short seq, const char * data = 0, unsigned int size = DATA_SIZE)
        : _header(type, code, id, seq) {
            if(data)
                memcpy(_data, data, size < DATA_SIZE ? size : DATA_SIZE);
            else
                memset(_data, 0, DATA_SIZE);
        }

        void checksum() {}

        unsigned char * data() { return _data; }

    private:
        Header _header;
        unsigned char  _data[DATA_SIZE];
    };

public:
    ICMP(IP * ip): _ip(ip) {
        _ip->attach(this, IP::ICMP);
    }

    ~ICMP() {
        _ip->detach(this, IP::ICMP);
    }

    void update(IP::Observed * ip, int c, NIC::Buffer * buf) {

//        Packet * packet = reinterpret_cast<Packet *>(dgram->data());
//
//        if(!check(packet)) {
//            db<ICMP>(TRC) << "ICMP::update() => checksum error!" << endl;
//            return;
//        }
//
//        if(packet.type() == ECHO) { // PONG
//            db<ICMP>(TRC) << "ICMP::echo sending automatic reply to " << src << endl;
//            Packet reply(ECHO_REPLY, 0, packet.id(), packet.sequence(), packet._data);
//            send(packet.dst(), packet.src(), reply);
//        }
//
//        if (packet.type() == ECHO_REPLY) {
//            db<ICMP>(TRC) << "ICMP::echo reply from " << src << endl;
//        }

        //         notify(src, dst, packet.type(), data, size);

    }
            
    void send(const Address & to, Packet & packet)
    {
        packet.checksum();
        _ip->send(to, IP::ICMP, &packet, sizeof(Packet));
    }

private:
    IP * _ip;
};

__END_SYS

#endif
