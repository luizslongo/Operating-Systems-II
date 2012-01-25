
#include <utility/ostream.h>
#include <ic.h>
#include <semaphore.h>

__USING_SYS

class RTSNOC {

public:
    typedef struct{
        unsigned int router_x;
        unsigned int router_y;
        unsigned int local;
    } Address;

    typedef struct{
        unsigned int local_addr;
        unsigned int router_x_addr;
        unsigned int router_y_addr;
        unsigned int net_x_size;
        unsigned int net_y_size;
        unsigned int data_width;
    } Info;

    RTSNOC():_info(){

        _info.local_addr = local_addr();
        _info.router_x_addr = router_x_addr();
        _info.router_y_addr = router_y_addr();
        _info.net_x_size = net_x_size();
        _info.net_y_size = net_y_size();
        _info.data_width = net_data_width();

        header_src_router_x_addr(_info.router_x_addr);
        header_src_router_y_addr(_info.router_y_addr);
        header_src_local_addr(_info.local_addr);

    }

    Info const& info() const { return _info;}

    void send_header(Address const* address);
    void send(Address const* address, unsigned int const* data);
    void send(unsigned int const* data);

    void receive_header(Address* address);
    void receive(Address* address, unsigned int* data);
    void receive(unsigned int* data);

    void receive_int(IC::Interrupt_Handler h);

private:
    Info _info;

private: //HW registers
    enum{
        NOC_BASE = Traits<Machine>::RTSNOC_ADDRESS,
    };

    enum{
        REG_HEADER_DST_LOCAL_ADDR = 0,
        REG_HEADER_DST_ROUTER_Y_ADDR,
        REG_HEADER_DST_ROUTER_X_ADDR,
        REG_HEADER_SRC_LOCAL_ADDR,
        REG_HEADER_SRC_ROUTER_Y_ADDR,
        REG_HEADER_SRC_ROUTER_X_ADDR,
        REG_DATA,
        REG_STATUS,
        REG_LOCAL_ADDR,
        REG_ROUTER_X_ADDR,
        REG_ROUTER_Y_ADDR,
        REG_NET_X_SIZE,
        REG_NET_Y_SIZE,
        REG_NET_DATA_WIDTH
    };


    static inline unsigned int reg(unsigned int offset){
        volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(NOC_BASE);
        return aux[offset];
    }

    static inline void reg(unsigned int offset, unsigned int val){
        volatile unsigned int* aux = reinterpret_cast<volatile unsigned int*>(NOC_BASE);
        aux[offset] = val;
    }

    static inline unsigned int header_src_router_x_addr(){return reg(REG_HEADER_SRC_ROUTER_X_ADDR);}
    static inline unsigned int header_src_router_y_addr(){return reg(REG_HEADER_SRC_ROUTER_Y_ADDR);}
    static inline unsigned int header_src_local_addr(){return reg(REG_HEADER_SRC_LOCAL_ADDR);}
    static inline unsigned int header_dst_router_x_addr(){return reg(REG_HEADER_DST_ROUTER_X_ADDR);}
    static inline unsigned int header_dst_router_y_addr(){return reg(REG_HEADER_DST_ROUTER_Y_ADDR);}
    static inline unsigned int header_dst_local_addr(){return reg(REG_HEADER_DST_LOCAL_ADDR);}

    static inline void header_src_router_x_addr(unsigned int val){reg(REG_HEADER_SRC_ROUTER_X_ADDR, val);}
    static inline void header_src_router_y_addr(unsigned int val){reg(REG_HEADER_SRC_ROUTER_Y_ADDR, val);}
    static inline void header_src_local_addr(unsigned int val){reg(REG_HEADER_SRC_LOCAL_ADDR, val);}
    static inline void header_dst_router_x_addr(unsigned int val){reg(REG_HEADER_DST_ROUTER_X_ADDR, val);}
    static inline void header_dst_router_y_addr(unsigned int val){reg(REG_HEADER_DST_ROUTER_Y_ADDR, val);}
    static inline void header_dst_local_addr(unsigned int val){reg(REG_HEADER_DST_LOCAL_ADDR, val);}

    static inline unsigned int data(){ return reg(REG_DATA);}
    static inline void data(unsigned int val){ reg(REG_DATA, val);}

    static inline void wr(){ reg(REG_STATUS, 0x1); }
    static inline void rd(){ reg(REG_STATUS, 0x2); }
    static inline bool wait(){return reg(REG_STATUS) & 0x4; }
    static inline bool nd(){return reg(REG_STATUS) & 0x8; }

    static inline unsigned int local_addr(){return reg(REG_LOCAL_ADDR);}
    static inline unsigned int router_x_addr(){return reg(REG_ROUTER_X_ADDR);}
    static inline unsigned int router_y_addr(){return reg(REG_ROUTER_Y_ADDR);}
    static inline unsigned int net_x_size(){return reg(REG_NET_X_SIZE);}
    static inline unsigned int net_y_size(){return reg(REG_NET_Y_SIZE);}
    static inline unsigned int net_data_width(){return reg(REG_NET_DATA_WIDTH);}


};

void RTSNOC::send_header(Address const* address){
    header_dst_local_addr(address->local);
    header_dst_router_x_addr(address->router_x);
    header_dst_router_y_addr(address->router_y);
}
void RTSNOC::send(unsigned int const* _data){
    while(wait());
     data(_data[0]);
     wr();
}
void RTSNOC::send(Address const* address, unsigned int const* _data){
    send_header(address);
    send(_data);
}
void RTSNOC::receive_header(Address* address){
    address->local = header_src_local_addr();
    address->router_x = header_src_router_x_addr();
    address->router_y = header_src_router_y_addr();
}
void RTSNOC::receive(unsigned int* _data){
    while(!nd());
    _data[0] = data();
    rd();
}
void RTSNOC::receive(Address* address, unsigned int* _data){
    receive_header(address);
    receive(_data);
}
void RTSNOC::receive_int(IC::Interrupt_Handler h){
    CPU::int_disable();
    IC::disable(IC::IRQ_NOC);
    IC::int_vector(IC::IRQ_NOC, h);
    IC::enable(IC::IRQ_NOC);
    CPU::int_enable();
}


OStream cout;
RTSNOC noc;
Semaphore rx_sem(0);

enum {
    ECHO_P0_LOCAL_ADDR = 0x6,
    ECHO_P1_LOCAL_ADDR = 0x4
};


void print_info(){

    RTSNOC::Info const& info = noc.info();

    cout << "NoC info: "
         << info.local_addr << ", "
         << info.router_x_addr << ", "
         << info.router_y_addr << ", "
         << info.net_x_size << ", "
         << info.net_y_size << ", "
         << info.data_width << "\n";
}

void send_pkt(unsigned int dst_addr, unsigned int data){
    cout << "TX: Sending pkt: DST_L=" << dst_addr << " DATA=" << data << "\n";
    RTSNOC::Address addr;
    addr.router_x = 0;
    addr.router_y = 0;
    addr.local = dst_addr;
    noc.send(&addr, &data);
    cout << "TX: Pkt sent\n";
    Thread::yield();
}

const unsigned int N_PKTS = 16;

int receive_pkt(){

    RTSNOC::Address addr;
    unsigned int data;

    for (unsigned int var = 0; var < N_PKTS; ++var){
        cout << "RX: Waiting pkt... \n";
        rx_sem.p();
        noc.receive(&addr, &data);
        cout << "RX: Pkt received:\n";
        cout << "RX:     Header: "
                << "DST_X=" << addr.router_x << ", "
                << "DST_Y="<< addr.router_y << ", "
                << "DST_L="<< addr.local << "\n";
        cout << "RX:     Data: " << data << "\n";
    }

    return 0;
}

void int_handler(unsigned int interrupt){
    //receive_pkt();
    rx_sem.v();
}

int main() {

    cout << "RTSNoC test\n\n";

    cout << "Setup interrupt and rx thread\n";
    noc.receive_int(&int_handler);

    Thread *rx_thead = new Thread(&receive_pkt);

    print_info();
    cout << "\n";

    for (unsigned int i = 0; i < N_PKTS/2; ++i) {
        send_pkt(ECHO_P0_LOCAL_ADDR, i);
        send_pkt(ECHO_P1_LOCAL_ADDR, ~i);
    }

    cout << "Waiting last packets\n";
    rx_thead->join();

    cout << "\nThe end!\n";

    *((volatile unsigned int*)0xFFFFFFFC) = 0;

    return 0;
}
