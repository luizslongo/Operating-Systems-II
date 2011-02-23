#ifndef __ip_h
#define __ip_h

#include <nic.h>
#include <arp.h>
#include <utility/malloc.h>
#include <utility/debug.h>
#include <utility/buffer.h>
#include <thread.h>

// Common aliases
typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;
typedef signed   long  s32;

__BEGIN_SYS
class IP_Address : public NIC_Common::Address<4> {
	public:
		IP_Address() {}
        IP_Address(unsigned char addr[4]) 
            : NIC_Common::Address<4>(addr[0],addr[1],addr[2],addr[3]) {}
            
        IP_Address(unsigned long addr) { *reinterpret_cast<u32 *>(this) = CPU::htonl(addr); }
		IP_Address(u8 a0, u8 a1 = 0,
		u8 a2 = 0, u8 a3 = 0)
		: NIC_Common::Address<4>(a0, a1, a2, a3) {}

		operator u32() { return *reinterpret_cast<u32 *>(this); }
		operator u32() const { return *reinterpret_cast<const u32 *>(this); }
		
	int strtoint(const char * s) {
		int res = 0;
		while (*s) {
			res = res * 10;
			res +=  *s++ - '0';
		}
		return res;
	}
	IP_Address(const char * _addr) {
		unsigned char addr[4];
		addr[0] = 0; addr[1] = 0; addr[2] = 0; addr[3] = 0;
		int i;
		for(i=0;i<4;++i) {
			char * sep = strchr(_addr,'.');
			bool last = !*sep;
			*sep = '\0';
			addr[i] = strtoint(_addr);
			_addr = ++sep;
			db<IP>(ERR) << "Addr: " << (unsigned int)(addr[i]) << endl;
			
			if (last) break;
		}
		memcpy(this,addr,sizeof(this));
	}
};

class IP: public NIC_Common, /*public NIC::Observer,*/ public Data_Observed<IP_Address>
{
public:

	typedef Data_Observer<IP_Address> Observer;
	typedef IP_Address                Address;
	typedef NIC::Address              MAC_Address;
	typedef u8                        Protocol;
	typedef ARP<NIC, IP>              _ARP;

	static const u16 MTU = ~0;
	static const Address BROADCAST;
	static const Address NULL;

	enum {
		PROT_IP   = NIC::IP,
		PROT_ARP  = NIC::ARP,
		PROT_RARP = NIC::RARP
	};

	class Header {
	public:
		enum {
			MF_FLAG = 1, // More Fragments
			DF_FLAG = 2  // Don't Fragment
		};

	public:
		Header() {}

		Header(const Address & src, const Address & dst, const Protocol & prot, u16 size) :
			_ihl(DEF_IHL), 	_version(DEF_VER), _tos(DEF_TOS),
			_length(CPU::htons(sizeof(Header) + size)), _id(CPU::htons(pktid++)),
			_offset(0), _flags(0), _ttl(DEF_TTL), _protocol(prot), _checksum(0),
			_src_ip(src), _dst_ip(dst)
		{
			calculate_checksum();
		}

		const Address & src_ip() const { return _src_ip; }
		const Address & dst_ip() const { return _dst_ip; }
		u32 hlength() { return _ihl * 4; }
		u32 length() const { return CPU::ntohs(_length); }
		u16 flags() const {
			return CPU::ntohs(_flags << 13 | _offset) >> 13;
		}
		u16 offset() const {
			return CPU::ntohs(_flags << 13 | _offset) & 0x1fff;
		}
		const Protocol & protocol() const { return _protocol; }
		u16 id() const { return CPU::ntohs(_id); }

		// setters for fragment operations
		void set_src(const Address & src_ip){ _src_ip = src_ip; }
		void set_length(u16 length) { _length = CPU::htons(length); }
		void set_offset(u16 off) {
			u16 x = CPU::htons(flags()<<13|off);
			_offset = x&0x1fff;
			_flags  = x>>13;
		}
		void set_flags(u16 flg) {
			u16 x = CPU::htons(flg<<13|offset());
			_offset = x&0x1fff;
			_flags  = x>>13;
		}
		void set_protocol(u8 protocol){
			_protocol = protocol;
		}

		void calculate_checksum();
		
		friend Debug& operator<< (Debug &db, const IP::Header &h) {
			IP::Address ip_src(h._src_ip), ip_dst(h._dst_ip);
			u16 flags = h.flags();

			db 	<< "{ver=" << h._version
			<< ",ihl=" << h._ihl
			<< ",tos=" << h._tos
			<< ",len=" << CPU::ntohs(h._length)
			<< ",id="  << CPU::ntohs(h._id)
			<< ",off=" << h.offset()
			<< ",flg=" << (flags == IP::Header::DF_FLAG ? "[DF]" : 
			(flags == IP::Header::MF_FLAG ? "[MF]" : "[ ]"))
			<< ",ttl=" << h._ttl
			<< ",pro=" << h._protocol
			<< ",chk=" << (void *)h._checksum
			<< ",src=" << ip_src
			<< ",dst=" << ip_dst
			<< "}";
			return db;
		}

	private:
		u8  _ihl:4;     // IP Header Length (in 32-bit words)
		u8  _version:4; // IP Version
		u8  _tos;       // Type Of Service (no used -> 0)
		u16 _length;    // Size of datagram (header + data)
		u16 _id;        // Datagram id
		u16 _offset:13; // Fragment offset (x 8 bytes)
		u16 _flags:3;   // Flags (UN, DF, MF)
		u8  _ttl;       // Time To Live
		Protocol  _protocol;  // RFC 1700 (1->ICMP, 6->TCP, 17->UDP)
		u16 _checksum;  // Header checksum
		Address _src_ip;    // Source IP address
		Address _dst_ip;    // Destination IP addres
	
		static unsigned short pktid;
	};

	MAC_Address arp(const Address & la);
	Address    rarp(const MAC_Address & pa);

	const Address & address() { return _self; }
	const Address & gateway() { return _gateway; }
	const Address & netmask() { return _netmask; }
	
	void set_address(const Address & addr) { _self = addr; }
	void set_gateway(const Address & addr) { _gateway = addr; }
	void set_netmask(const Address & addr) { _netmask = addr; }

	s32 send(const Address & to,const char * data,u16 size,Protocol proto) {
		SegmentedBuffer sb(data,size);
		return send(_self,to,&sb,proto);
	}

	s32 send(const Address & to,SegmentedBuffer * data,Protocol proto) {
		return send(_self,to,data,proto);
	}

	s32 send(const Address & from,const Address & to,SegmentedBuffer * data,Protocol proto);

	//TODO: unit not working
	IP(unsigned int unit=0);
	~IP();

//	void update(NIC::Observed * o, int p);

	void worker_loop();
	void process_incoming();
	void process_ip(char * data,u16 size);

	void kill();

	NIC * nic() { return &_nic; }
	const MAC_Address & hw_address() { return _nic.address(); }
	const int hw_address_len() { return sizeof(MAC_Address); }
	const u16 mtu() { return _nic.mtu(); }

	static u16 calculate_checksum(void* ptr, u16 count);
	
	static IP* instance(unsigned int unit = 0) { 
		if (!_instance[unit])
			_instance[unit] = new IP(unit);
		return _instance[unit]; 
	}

	static int thread_main(IP * thiz);
private:
	NIC _nic;

	_ARP::Table _arpt;
	
	Address _self;
	Address _netmask;
	Address _gateway;
	Address _broadcast;

	volatile bool _alive;
	Thread * _thread;

	static const unsigned int DEF_VER = 4;
	static const unsigned int DEF_IHL = 5;    // 20 bytes
	static const unsigned int DEF_TOS = 0;
	static const unsigned int DEF_TTL = 0x40; // 64 hops

	// Pseudo header for checksum calculations
	struct Pseudo_Header {
		u32 src_ip;
		u32 dst_ip;
		u8 zero;
		u8 protocol;
		u16 length;
	};
	
	static IP* _instance[Traits<NIC>::NICS::Length];
};

__END_SYS

#endif

