#ifndef __udp_h
#define __udp_h

#include <ip.h>
#include <semaphore.h>

__BEGIN_SYS

class UDP : public IP::Observer, public IP {
public:
	// UDP ID (IP Frame)
	static const unsigned int ID_UDP = 0x11;

	class Address : public IP::Address{
	public:
		Address(){}
		Address(char *ip, u16 port) : IP::Address(ip), _port(port) {}
		Address(u16 port) : IP::Address(IP::self()), _port(port) {}
		Address(u32 ip, u16 port) : IP::Address(ip), _port(port) {}
		Address(IP::Address ip, u16 port) : IP::Address(ip), _port(port) {}

		friend Debug& operator << (Debug& db, const Address & h) {
			return print_common(db, h);
		}

		friend OStream& operator << (OStream& out, const Address &h){
			return print_common(out, h);
		}

		u16 port() const { return _port; }

		// To be used by UDP's contructor when a port
		// is already busy
		void change_port(u16 new_port){
			_port = new_port;
		}

	private:
		u16 _port;

		template <typename T>
		static T& print_common(T& out, const Address &h){
			out << static_cast<IP::Address>(h) << ":" 
			   << h._port;
			return out;
		}
	};

	class Header{
	public:
		Header(){}
		Header(u16 src_port=0, u16 dst_port, u16 data_size) :
			_src_port(CPU::htons(src_port)), 
			_dst_port(CPU::htons(dst_port)), 
			_length(CPU::htons(sizeof(UDP::Header) + data_size)),
			_checksum(0) { }
		
		void checksum(u16 cs) { _checksum = cs; }
		u16 dst_port() const { return CPU::ntohs(_dst_port); }
		u16 src_port() const { return CPU::ntohs(_src_port); }
		friend Debug & operator << (Debug & db, const Header & h){
			db 	<< "{sprt=" << CPU::ntohs(h._src_port)
				<< ",dprt=" << CPU::ntohs(h._dst_port)
				<< ",len="  << CPU::ntohs(h._length)
				<< ",chk="  << (void *)h._checksum
				<< "}";
			return db;
		}
		
	private:
		u16 _src_port; // Source UDP port
		u16 _dst_port; // Destination UDP port
		u16 _length;   // Length of datagram (header + data) in bytes
		u16 _checksum; // Pseudo header checksum (see RFC)
	};

	class PDU {
	public:
		PDU(){}

		PDU(const Address &src, const Address &dst, const void *data,
			u16 data_size) : _ip_header(src, dst, ID_UDP, 
			sizeof(UDP::Header) + data_size),
			_udp_header(src.port(), dst.port(), data_size) {

			calculate_checksum();
			memcpy(_data, data, data_size);
			db<PDU>(TRC) << "UDP Packet Created - IPHeader="
				<< _ip_header << " UDPHeader=" << _udp_header << "\n";
		}

		unsigned int size() const { return _ip_header.length(); }
		u8 *data(){ return _data; }

		friend Debug & operator << (Debug & db, const PDU & d) {
			db << "{hdr= IPH=" << d._ip_header 
			   << " UDPH="<< d._udp_header << "}";
	    		return db;
		}
	private:
		void calculate_checksum();
		IP::Header _ip_header;
		UDP::Header _udp_header;
#ifndef __avr8_h
		u8 _data[IP::MTU - sizeof(UDP::Header) - sizeof(IP::Header)];
#else
		u8 _data[256];
#endif
	};

	UDP() : _semaph(0), _self(_port++) { constructor_common(); }
	UDP(u16 port) : _semaph(0), _self(port) {constructor_common();}
	UDP(Address &a) : _semaph(0), _self(a) {constructor_common();}

	~UDP(){ 
		_busy_ports.remove(_link); 
		db<UDP>(TRC) << "UDP::~UDP() self=" << _self << "\n";
	}

	void received(u32 src, void *data, u16  size);
	int receive(UDP::Address &src, void *data, unsigned int size);

	Address self() const{
		return _self;
	}

private:

	void constructor_common();

	u16 check_port(u16 to_check){
		Simple_Ordered_List<UDP, u16>::Element *e = _busy_ports.head();
		for(;e;e=e->next()){
			if(to_check == e->object()->self().port()){
				u16 busy = e->object()->self().port();
				for(e = e->next(); e; e->next())
					if(++busy < e->object()->self().port())
						return busy;
			}
		}
		db<UDP>(INF) << "\n";
		return 0;
	}

	class Receive_Entry{
	public:
		Receive_Entry(u16 port, Semaphore &s, Address &src, 
				void *data, unsigned int size) :
			_port(port),
			_s(s),
			_src(src),
			_data(data),
			_size(size),
			_link(this) {}

		u16 _port;
		Semaphore &_s;
		Address &_src;
		void *_data;
		unsigned int _size;
		Simple_List<Receive_Entry>::Element _link;

	};
	
	typedef Simple_Ordered_List<UDP, u16> Ports_List;
	
	Semaphore _semaph;
	Address _self;
	Ports_List::Element *_link;
	
	static u16 _port;
	static Ports_List _busy_ports;
	static Simple_List<Receive_Entry> _receive_list;

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

