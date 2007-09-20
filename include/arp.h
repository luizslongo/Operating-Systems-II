#ifndef __arp_h
#define __arp_h

#include <utility/malloc.h>
#include <cpu.h>
#include <nic.h>
#include <alarm.h>
#include <condition.h>
#include <ip.h> // just to print the formatted IP address for debug purposes

__BEGIN_SYS

class ARP : public NIC::Observer, private Protocol_Common {
public:

	typedef NIC::Address HAddress; //hardware address
	typedef IP::Address  LAddress;  //logic address
	
	static HAddress BCAST;
	static HAddress NULL_ADDR;

	// NIC::Observer's pure virtual function implementation
	void received(void *data, unsigned int size);

	// Returns the hardware address related to some logical one
	// by broadcasting the request through the network.
	const HAddress &arp(LAddress laddr_dst);

	ARP(NIC &nic, LAddress me);

	class ARP_Packet{
	public:
		static const u16 HTYPE_ETHERNET = 1;
		static const u16 PTYPE_IP = 0x800;
		static const u8 HLEN_ETHERNET = 6;
		static const u8 PLEN_IP = 4;
	
		typedef enum{ ARP_REQ   = 1 } Arp_Request;
		typedef enum{ ARP_RPLY  = 2 } Arp_Reply;
		typedef enum{ RARP_REQ  = 3 } Rarp_Request;
		typedef enum{ RARP_RPLY = 4 } Rarp_Reply;
	
		ARP_Packet(Arp_Request op, HAddress ha_src, LAddress la_src, LAddress la_dst) :
			_htype(CPU::htons(HTYPE_ETHERNET)),
			_ptype(CPU::htons(PTYPE_IP)),
			_hlen(HLEN_ETHERNET),
			_plen(PLEN_IP),
			_opcode(CPU::htons(op)),
			_tla(la_dst){ // what is the hw addr of this logic addr
			memcpy(&_sla, &la_src,4);
			memcpy(&_sha,&ha_src,6);
			memset(&_tha, 0, 6);
		}
	
		ARP_Packet(Arp_Reply op, HAddress ha_src, LAddress la_src, HAddress ha_dst, LAddress la_dst) :
			_htype(CPU::htons(HTYPE_ETHERNET)),
			_ptype(CPU::htons(PTYPE_IP)),
			_hlen(HLEN_ETHERNET),
			_plen(PLEN_IP),
			_opcode(CPU::htons(op)) {
			memcpy(&_sha,&ha_src,6);   // This hw addr
			memcpy(&_sla, &la_src,4);  // has this logic addr
			memcpy(&_tha, &ha_dst, 6);
			memcpy(&_tla, &la_dst, 4);
		}
	
		ARP_Packet(Rarp_Request op, HAddress ha_src) :
			_htype(CPU::htons(HTYPE_ETHERNET)),
			_ptype(CPU::htons(PTYPE_IP)),
			_hlen(HLEN_ETHERNET),
			_plen(PLEN_IP),
			_opcode(CPU::htons(op)) {
			memset(&_sla, 0, 4);
			memcpy(&_sha, &ha_src, 6);
			memcpy(&_tha, &ha_src, 6); // what is the logic addr of this hw addr
			memset(&_tla, 0, 4);
		}
	
	//	ARP_Packet(Rarp_Reply op) :
	//		_opcode(op) {}
	

		u16 opcode()  { return CPU::ntohs(_opcode); }
		HAddress sha(){ 
			char *h = (char*)&_sha;
			HAddress haddr(h[0],h[1],h[2],h[3],h[4],h[5]);
			return haddr;
		}
		HAddress tha(){ 
			char *h = (char*)&_tha;
			HAddress haddr(h[0],h[1],h[2],h[3],h[4],h[5]);
			return haddr;
		}
		LAddress sla(){ return *((LAddress*)&_sla); }
		LAddress tla(){ return *((LAddress*)&_tla); }

		friend Debug &operator <<(Debug &db,const ARP_Packet &a);
	
	private:
		u16 _htype;  // Hardware Type
		u16 _ptype;  // Protocol Type
		u8  _hlen;   // Hardware Address Length
		u8  _plen;   // Protocol Address Length
		u16 _opcode;
		u32 _sha;    // Sender Hardware Address
		u16 _sha2;
		u16 _sla;
		u16 _sla2;
		u16 _tha;
		u32 _tha2;
		u32 _tla;

	};

private:
	class ARP_Table{
		public:
			ARP_Table(){}
			bool get(LAddress laddr, HAddress *&haddr);
			void put(LAddress laddr, HAddress haddr);
			Condition * add(LAddress laddr);

		private:
			class Entry{
			public:
				Entry( LAddress laddr, HAddress haddr) :
					_laddr(laddr),
					_haddr(haddr),
					_link(this) {}

				LAddress _laddr;
				HAddress _haddr;
				Condition _c;
				Simple_List<Entry>::Element _link;
			};

			

			Entry* search(LAddress laddr);

			typedef Simple_List<Entry> List;
			List _table;
	};


	class Handler_Condition : public Handler{
	public:
		Handler_Condition(Condition *c) : _c(c){}
		void operator()() { _c->signal(); }
	private:
		Condition *_c;
	};

	static ARP_Table _arp_tab;

	NIC &_nic;
	HAddress _hmyself;
	LAddress _lmyself;
};

__END_SYS

#endif

