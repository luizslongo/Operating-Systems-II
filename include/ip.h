#ifndef __ip_h
#define __ip_h

#include <utility/malloc.h>
#include <utility/debug.h>
#include <cpu.h>
#include <nic.h>

__BEGIN_SYS

class IP : public Protocol_Common, public NIC::Observer {
public:
	// IP ID (Ethernet frame type)
	static const unsigned int ID = Ethernet_NIC::IP;

	// IP MTU
	static const unsigned int MTU = 64 * 1024;   
	static const unsigned int PROT_ANY =  0x3f;

	class Address {
	public:
		Address(){}
		Address(const Address &a) : _address(a._address) {}
		Address(char *ip) : _address(str2bytes(ip)) {}
		Address(u32 addr) : _address(addr) {}
		static u32 str2bytes(char *ip);

		operator u32() const { return _address; }

		friend Debug& operator<< (Debug &out, const Address &a){
			return print_common(out, a);
		}

		friend OStream& operator<< (OStream &out, const Address &a){
			return print_common(out, a);
		}

	private:
		// Address in the network format (big endian)
		u32 _address;

		template<typename T>
		static T& print_common(T& out, const Address &a){
			char *addr = (char*)&a._address;
			out     << (int)((unsigned char)addr[0]) << "."
				<< (int)((unsigned char)addr[1]) << "."
				<< (int)((unsigned char)addr[2]) << "."
				<< (int)((unsigned char)addr[3]);
			return out;		
		}
	};
	
	static Address BROADCAST;

	class Header{
	public:
		static const u16 MF_FLAG = 1; // More Fragments
		static const u16 DF_FLAG = 2; // Don't Fragment
		
		Header() {}
		Header(u32 src_ip, u32 dst_ip, u16 protocol, u16 data_size) : 
			_ihl(DEF_IHL), 
			_version(DEF_VER),
			_tos(DEF_TOS),
			_length(CPU::htons(sizeof(Header) + data_size)),
			_id(CPU::htons(generate_id())),
			_offset(0),
			_flags(0),
			_ttl(DEF_TTL),
			_protocol(protocol),
			_checksum(0),
			_src_ip(src_ip),
			_dst_ip(dst_ip) { calculate_checksum(); }

		u32 src_ip() const { return _src_ip; }
		u32 dst_ip() const { return _dst_ip; }
		u8  hlength(){ return _ihl * sizeof(u32); } // header length in bytes
		u16 length() const { return CPU::ntohs(_length); }
		u16 flags()  const { return CPU::ntohs(_flags<<13|_offset)>>13; }
		u16 offset() const { return CPU::ntohs(_flags<<13|_offset)&0x1fff; }
		u8 protocol() const { return _protocol; }
		u16 id() const { return CPU::ntohs(_id); }

		// setters for fragment operations
		void set_src(u32 src_ip){ _src_ip = src_ip; }
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
	
		friend Debug & operator << (Debug & db, const Header & h);

	private:
		u8  _ihl:4;     // IP Header Length (in 32-bit words)
		u8  _version:4; // IP Version
		u8  _tos;       // Type Of Service (no used -> 0)
		u16 _length;    // Size of datagram (header + data)
		u16 _id;        // Datagram id
		u16 _offset:13; // Fragment offset (x 8 bytes)
		u16 _flags:3;   // Flags (UN, DF, MF)
		u8  _ttl;       // Time To Live
		u8  _protocol;  // RFC 1700 (1->ICMP, 6->TCP, 17->UDP)
		u16 _checksum;  // Header checksum (the checksum field
				// is the 16 bit one's complement of the
				// one's complement sum of all 16 bit
				// words in the header. For purposes of 
				// computing the checksum, the value of
				// the checksum field is zero.
		u32 _src_ip;    // Source IP address
		u32 _dst_ip;    // Destination IP addres

	};

	class PDU{
	public:
		PDU() {}

		PDU(u32 src_ip, u32 dst_ip, const void * data, u16 data_size) :
				_header(src_ip, dst_ip, PROT_ANY, data_size) {
			memcpy(_data, data, data_size); 
		}
		PDU(u32 src_ip, u32 dst_ip, u16 protocol, const void * data, 
			u16 data_size) : _header(src_ip, dst_ip, protocol, data_size) {
			memcpy(_data, data, data_size); 
		}

		Header &header() { return _header; }
		unsigned int size() const { return _header.length(); }

		// for fragmentation purposes
		u8 *data(){
			return _data;
		}

		void set_data_fragment(const PDU &pdu, u16 offset, u16 size){
			memcpy(_data, pdu._data+offset, size);
		}

		void headercpy(const PDU &d){
			_header = d._header;
		}

		friend Debug & operator << (Debug & db, const PDU & d) {
			db << "{hdr=" << d._header << "}";
			return db;
		}

	private:
		Header _header;
#ifndef __avr8_h
		u8 _data[MTU - sizeof(Header)];
#else
		u8 _data[256];
#endif
	};

	// Abstract class to be implemented by IP's observers
	class Observer{
	public:
		virtual void received(u32 src, void *data, u16 size) = 0;
		virtual ~Observer(){}
	};

	// NIC::Observer's callback function implementation.
	void received(void *data, unsigned int size);

	static Address self(){ return _self; }

	// Inserts an IP Observer to observer's list
	static void attach(u16 protocol, Observer &ipo);
	
	// As this class is scenario independent, it doesn't send the data. 
	// So, it offers this function do mount each fragment to be sent by 
	// someone else.
	static bool get_fragment(u16 i, u16 mtu, void *pdu_, void *frag_);

	u32 logic_address(){
		return _self;
	}


	IP(){
		// In order to guarantee that NIC::attach will be called just once
		// for IP protocol. Inspired by the Singleton Design Pattern.
		if(!_instance) NIC::attach(ID, *(_instance = this));
	}

private:
	class Protocol_Entry{
	public:
		Protocol_Entry(u16 protocol, Observer &obs) :
			_ip_protocol(protocol), _o(obs), _link(this) {}

		u16 _ip_protocol;
		Observer &_o;
		Simple_List<Protocol_Entry>::Element _link;
	};

	
	class Reassembly_Entry{
	public:
		typedef Grouping_List<char>::Element Element;
		typedef Grouping_List<char> List;

		Reassembly_Entry(u16 id) :
			_id(id), _packet_size(0xffff),
			_link(this) {}

		u16 _id;
		u16 _packet_size;
		PDU _pdu;
		List _frag_list;
		Simple_List<Reassembly_Entry>::Element _link;
	};

	static IP *_instance;
	static Address _self;

        static const unsigned int DEF_VER = 4;
        static const unsigned int DEF_IHL = 5;    // 20 bytes
        static const unsigned int DEF_TOS = 0;
        static const unsigned int DEF_TTL = 0x40; // 64 hops

	static u16 generate_id() { return _next_id++; }
	static u16 _next_id;

	static Simple_List<Protocol_Entry> _ip_observers;
	static Simple_List<Reassembly_Entry> _reassembly_table;

	static bool new_fragment(Reassembly_Entry &entry, PDU &fragment);
	
	// Call received() function of the observers whose protocol match with protocol's field on IP header
	static void notify(u16 protocol, u32 src, void *data, u16 size);

	// Pseudo header for checksum calculations
	struct Pseudo_Header {
		u32 src_ip;
		u32 dst_ip;
		u8 zero;
		u8 protocol;
		u16 length;
	};
	
};

__END_SYS

#endif

