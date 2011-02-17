#ifndef __udp_h
#define __udp_h

#include "ip.h"

__BEGIN_SYS 

class UDP_Address:public IP_Address {
 public:
	UDP_Address() {	}
	UDP_Address(u16 port):IP_Address(IP::NULL), _port(port) { }
	UDP_Address(u32 ip, u16 port):IP_Address(ip), _port(port) { }
	UDP_Address(IP_Address ip, u16 port):IP_Address(ip), _port(port) { }

	friend Debug & operator <<(Debug & db, const UDP_Address & h) {
		return print_common(db, h);
	}

	friend OStream & operator <<(OStream & out, const UDP_Address & h) {
		return print_common(out, h);
	}

	u16 port() const {
		return _port;
	}
	IP_Address ip() const {
		return static_cast < IP_Address > (*this);
	}
	void change_port(u16 new_port) {
		_port = new_port;
	}

 private:
	u16 _port;

	template < typename T >
	    static T & print_common(T & out, const UDP_Address & h) {
		out << h << ":" << h._port;
		return out;
	}
};

class UDP: public IP::Observer, public Data_Observed < UDP_Address > {
 protected:
	void calculate_checksum();

 public:
	// UDP ID (IP Frame)
	static const IP::Protocol ID_UDP = 0x11;

	typedef UDP_Address Address;

	class Header {
	 public:
		Header() {} 

		Header(u16 src_port = 0, u16 dst_port = 0, u16 data_size = 0):
			_src_port(CPU::htons(src_port)),
			    _dst_port(CPU::htons(dst_port)),
			    _length(CPU::htons(sizeof(UDP::Header) + data_size)),
			    _checksum(0)
		{ }
		void checksum(u16 cs) {
			_checksum = cs;
		}
		u16 dst_port() const {
			return CPU::ntohs(_dst_port);
		}
		u16 src_port() const {
			return CPU::ntohs(_src_port);
		}
		friend Debug & operator <<(Debug & db, const Header & h) {
			db << "{sprt=" << CPU::ntohs(h._src_port)
			    << ",dprt=" << CPU::ntohs(h._dst_port)
			    << ",len=" << CPU::ntohs(h._length)
			    << ",chk=" << (void *)h._checksum << "}";
			return db;
		}

	 private:
		u16 _src_port;	// Source UDP port
		u16 _dst_port;	// Destination UDP port
		u16 _length;	// Length of datagram (header + data) in bytes
		u16 _checksum;	// Pseudo header checksum (see RFC)
	};

	UDP(IP * ip) : _ip(ip) {
		_ip->attach(this, ID_UDP);
	}

	~UDP() {
		_ip->detach(this, ID_UDP);
	}

	s32 send(Address local, Address remote, SegmentedBuffer * data);

	class Socket: public Data_Observer <UDP_Address> {
		friend class UDP;
	 protected:
		UDP::Address _local;
		UDP::Address _remote;

		UDP *_udp;
	 public:
		Socket(UDP * udp, UDP::Address local, UDP::Address remote)
		:_local(local), _remote(remote), _udp(udp) {
			_udp->attach(this, _local.port());
		}

		~Socket() {
			_udp->detach(this, _local.port());
		}

		s32 send(const char *data, u16 size) {
			SegmentedBuffer sb(data, size);
			return send(&sb);
		}
		s32 send(SegmentedBuffer * data) {
			db<IP>(TRC) << "UDP::Socket::send()\n";
			return _udp->send(_local, _remote, data);
		}

		void set_remote(const UDP::Address & party) {
			_remote = party;
		}
		const Address & remote() {
			return _remote;
		}

		void set_local(const UDP::Address & local) {
			//TODO: deatach/attach here
			_local = local;
		}

		// every Socket should implement one
		virtual void received(const UDP::Address & src,
				      const char *data, unsigned int size) = 0;

		void update(Observed * o, long c,UDP_Address src,
			     UDP_Address dst,void * data,unsigned int size);
	};

	// Data_Observer callback
	void update(Data_Observed<IP::Address> *ob, long c, IP::Address src,
		    IP::Address dst, void *data, unsigned int size);

	IP *ip() {
		return _ip;
	}

 private:

	void constructor_common();

	struct Pseudo_Header {
		u32 src_ip;
		u32 dst_ip;
		u8 zero;
		u8 protocol;
		u16 length;
	};

	IP *_ip;
};

__END_SYS
#endif
