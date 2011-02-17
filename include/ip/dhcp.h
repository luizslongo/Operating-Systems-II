#ifndef DHCPC_H
#define DHCPC_H

#include "udp.h"
#include <utility/random.h>

__BEGIN_SYS
/*
 * Reference:
 * DHCP:         http://www.ietf.org/rfc/rfc2131.txt
 * DHCP options: http://www.ietf.org/rfc/rfc1533.txt
 */
class DHCP {
public:
	// Limited to opt_size = 308

	template<int opt_size> class Packet {
	public:
			u8 _op, _htype, _hlen, _hopts;
			u32 _xid;
			u16 _secs, _flags;
			u32 _ciaddr, _yiaddr, _siaddr, _giaddr;
			u8 _chaddr[16];
			u8 _sname[64];
			u8 _file[128];
			u8 _magic[4];
			u8 _options[opt_size];
			u8 _end;
			u8 _padding[312 - 5 - opt_size];

			u8 op() const { return _op; }
			u32 xid() const { return _xid; }
			u16 secs() const { return CPU::ntohs(_secs); }
			u32 your_address() const { return _yiaddr; }
			u32 server_address() const { return _siaddr; }
			u8 * options() const { _options; }

			Packet()
			{
				memset(&_op, 0, sizeof(Packet));
				_magic[0] =  99; // magic cookie
				_magic[1] = 130;
				_magic[2] =  83;
				_magic[3] =  99;
				_end = 255; // end of options
			}

	};

	class Discover : public Packet<3> {
	public:
		Discover(IP * _net) : Packet<3>() {
			_op = 1;
			_htype = 1;
			_hlen = _net->hw_address_len();
			_xid = Pseudo_Random::random();
			memcpy(_chaddr, &_net->hw_address(), _hlen);
			_options[0] = 53; // DHCPMSG
			_options[1] = 1;  // message size
			_options[2] = 1;  // dhcp discover
		}

	};

	class Request : public Packet<8> {
	public:
		Request(IP * _net,const Packet<255> * discovered) : Packet<8>() {
			_op = 1;
			_htype = 1;
			_hlen = _net->hw_address_len();
			_xid = discovered->_xid;
			_ciaddr = discovered->_ciaddr;
			_siaddr = discovered->_siaddr;
			memcpy(_chaddr, &_net->hw_address(), _hlen);
			_options[0] = 53; // DHCP message
			_options[1] = 1;  // size
			_options[2] = 3;  // dhcp discover
			_options[3] = 55; // parameter request
			_options[4] = 3;  // size
			_options[5] = 1;  // subnet
			_options[6] = 3;  // router
			_options[7] = 6;  // dns
		}
	};

	class Client;
};

class DHCP::Client : public UDP::Socket {
public:
	enum {
		IDLE,
		DISCOVER,
		REQUEST,
		RENEW,
		RELEASE
	};
protected:
	short _state;

	u32 _xid;

	IP::Address _ip, _mask, _gw, _bcast, _ns;
public:
	Client(UDP * udp) :
		UDP::Socket(udp,UDP::Address(0,68),UDP::Address(~0,67)),
		_state(IDLE)
	{

	}

	~Client() {}

	// UDP::Socket virtual methods
	void received(const UDP::Address & src,const char *data, unsigned int size)
	{
		db<IP>(INF) << "DHCP::Client state: " << _state << "\n";
		const DHCP::Packet<255> * packet = reinterpret_cast<const DHCP::Packet<255> *>(data);

		if (_xid != packet->xid()) {
			db<IP>(TRC) << "This DHCP message does not belong to me\n";
		}

		switch(_state) {
		case DISCOVER:

			if (packet->your_address()) {
				set_remote(src);

				UDP::Address me(packet->your_address(),68);
				set_local(me);
				_udp->ip()->set_address(me.ip());

				_state = REQUEST;
				db<IP>(INF) << "Server " << src.ip() << " offered IP " << me.ip() << "\n";

				DHCP::Request req(_udp->ip(),packet);
				SegmentedBuffer sb(&req,sizeof(DHCP::Request));

				send(&sb);
			}

			break;

		case REQUEST:

			if (packet->your_address()) {
				_ip = IP::Address((u32)packet->your_address());

				_state = RENEW;


			}
		}
	}

	void configure() {
		db<IP>(INF) << "DHCP::Client sending discover msg\n";
		_state = DISCOVER;

		DHCP::Discover pkt(_udp->ip());
		SegmentedBuffer sb(&pkt,sizeof(DHCP::Discover));

		_xid = pkt.xid();
		send(&sb);
	}

	void renew() {

	}

	void release() {

	}

	IP::Address ip() { return _ip; }
	IP::Address netmask() { return _mask; }
	IP::Address gateway() { return _gw; }
	IP::Address brodcast() { return _bcast; }
	IP::Address nameserver() { return _ns; }
};

__END_SYS

#endif
