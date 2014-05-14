//// EPOS ICMP Protocol Implementation
//
//#include <icmp.h>
//
//__BEGIN_SYS
//
//ICMP::ICMP()
//{
//    attach(this, ICMP_ID);
//}
//
//ICMP::~ICMP()
//{
//    detach(this, ICMP_ID);
//}
//
//void ICMP::update(Data_Observed<IP::Address> *ob, long c, IP::Address src,
//                  IP::Address dst, void *data, unsigned int size)
//{
//    Packet& packet = *reinterpret_cast<Packet*>(data);
//    if (IP::calculate_checksum(data,size) != 0xFFFF) {
//        db<ICMP>(TRC) << "ICMP::checksum error\n";
//        return;
//    }
//
//    if (Traits<ICMP>::echo_reply && (packet.type() == ECHO)) { // PONG
//        db<ICMP>(TRC) << "ICMP::echo sending automatic reply to " << src << endl;
//        Packet reply(ECHO_REPLY,0,packet.id(),packet.sequence(),packet._data);
//        send(dst,src,reply);
//    }
//
//    if (packet.type() == ECHO_REPLY) {
//        db<ICMP>(TRC) << "ICMP::echo reply from " << src << endl;
//    }
//
//    notify(src,dst,packet.type(),data,size);
//}
//
//
//void ICMP::send(Address from, Address to, Packet & pkt)
//{
//    pkt._checksum = 0;
//    pkt._checksum = ~(checksum(&pkt, sizeof(pkt)));
//    SegmentedBuffer sb(pkt, sizeof(pkt));
//    IP::send(from, to, &sb, ICMP_ID);
//}
//
//__END_SYS
