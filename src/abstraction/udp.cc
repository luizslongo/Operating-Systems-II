#include <ip/udp.h>

__BEGIN_SYS


void UDP::calculate_checksum() {
	// BELLOW CODE IS BUGGED, Will be corrected soon
//	if(false) {
//		unsigned int len = size();
//
//		Pseudo_Header phdr;
//		phdr.src_ip = _ip_header.src_ip();
//		phdr.dst_ip = _ip_header.dst_ip();
//		phdr.zero = 0;
//		phdr.protocol = ID_UDP;
//		phdr.length = CPU::htons(len);
//
//		unsigned int sum = 0;
//
//		u8 * ptr = reinterpret_cast<u8 *>(this);
//		unsigned int i;
//
//		for(i = 0; i < len-1; i+=2)
//			sum += (((u16)(ptr[i+1]) & 0x00FF) << 8) | ptr[i];
//		if(len & 1) {
//			sum += ptr[len-1];
//		}
//
//		ptr = reinterpret_cast<u8 *>(&phdr);
//		for(i = 0;i < sizeof(Pseudo_Header); i+=2)
//			sum += (((u16)(ptr[i+1]) & 0x00FF) << 8) | ptr[i];
//
//		while(sum >> 16)
//			sum = (sum & 0xffff) + (sum >> 16);
//
//
//		_udp_header.checksum(~sum);
//	} else
//		_udp_header.checksum(0);
}

// Assembles data and sends to IP layer

s32 UDP::send(Address _local, Address _remote, SegmentedBuffer * data) {
	UDP::Header hdr(_local.port(), _remote.port(),
			data->total_size());
	SegmentedBuffer sb(&hdr, sizeof(UDP::Header), data);
	hdr.checksum(0);	//TODO: change calculate_checksum() to accept SegmentedBuffers
	return _ip->send(_local.ip(), _remote.ip(), &sb, ID_UDP) - 8;	// discard header
}

// Called by IP's notify(...)

void UDP::update(Data_Observed<IP::Address> *ob, long c, IP::Address src,
	         IP::Address dst, void *data, unsigned int size)
{
	Header& hdr = *reinterpret_cast<Header*>(data);

	db<IP>(INF) << "UDP::update: received "<< size <<" bytes from " 
	            << src << " to " << dst << "\n";

	// TODO: put checksum verification here
	notify(UDP::Address(src,hdr.src_port()),UDP::Address(dst,hdr.dst_port()),
	       (int) hdr.dst_port(), &((char*)data)[sizeof(Header)],
	       size - sizeof(Header));
}

// Called by UDP's notify(...)

void UDP::Socket::update(Observed *o, long c, UDP_Address src, UDP_Address dst,
                         void *data, unsigned int size)
{
	db<IP>(TRC) << __PRETTY_FUNCTION__ << "\n";
	
	// virtual call
	received(src,(const char*)data,size);
}

__END_SYS

