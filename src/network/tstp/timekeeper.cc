// EPOS Trustful Space-Time Protocol Implementation

#include <utility/math.h>
#include <utility/string.h>
#include <machine/nic.h>
#include <network/tstp/tstp.h>

#ifdef __tstp__

__BEGIN_SYS

TSTP::Time TSTP::Timekeeper::_reference;
volatile TSTP::Time_Stamp TSTP::Timekeeper::_next_sync;
Function_Handler * TSTP::Timekeeper::_life_keeper_handler;
Alarm * TSTP::Timekeeper::_life_keeper;


TSTP::Timekeeper::~Timekeeper()
{
    db<TSTP>(TRC) << "TSTP::~Timekeeper()" << endl;

    detach(this);
}

void TSTP::Timekeeper::update(Data_Observed<Buffer> * obs, Buffer * buf)
{
    Header * header = buf->frame()->data<Header>();
    db<TSTP>(TRC) << "TSTP::Timekeeper::update(obs=" << obs << ",buf=" << buf << " = " << *header << ")" << endl;
    if(buf->is_microframe) {
        if(sync_required())
            buf->relevant = true;
    } else {
        buf->deadline = Router::destination(buf).t1;
        bool peer_closer_to_sink = buf->downlink ? (here() - sink() > header->last_hop().space - sink()) : (buf->my_distance > buf->sender_distance);

        if(header->time_request()) {
            db<TSTP>(TRC) << "TSTP::Timekeeper::update: time_request received" << endl;
            // Respond to Time Request if able
            if(synchronized() && !peer_closer_to_sink) {
                db<TSTP>(TRC) << "TSTP::Timekeeper::update: responding to time request" << endl;
                keep_alive();
            } else {
                if(peer_closer_to_sink) {
                    Time_Stamp t0 =header->last_hop().time + NIC_TIMER_INTERRUPT_DELAY;
                    Time_Stamp t1 = buf->sfdts;

                    Configuration conf;
                    conf.selector = Configuration::TIMER;
                    conf.timer_frequency = 0; // unchanged
                    conf.parameter = t0 - t1;
                    _nic->reconfigure(&conf);

                    _next_sync = time_stamp() + sync_period();
                    _life_keeper->reset();

                    db<TSTP>(TRC) << "TSTP::Timekeeper::update: adjusted timer offset by " << conf.parameter << endl;
                    db<TSTP>(INF) << "TSTP::Timekeeper::update:now= " << now() << endl;
                }
            }
        }
    }
}


// Class Methods
void TSTP::Timekeeper::marshal(Buffer * buf)
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::marshal(buf=" << buf << ")" << endl;

    Header * header = buf->frame()->data<Header>();

    header->origin(now());
    header->time_request(sync_required());
    if((header->type() == CONTROL) && (header->subtype() == KEEP_ALIVE)) {
    	header->time_request( true );
    	buf->deadline = now() + sync_period();
    }
    else
        buf->deadline = Router::destination(buf).t1; // deadline must be set after origin time for Security messages
}


void TSTP::Timekeeper::keep_alive()
{
    db<TSTP>(TRC) << "TSTP::Timekeeper::keep_alive()" << endl;

    Buffer * buf = alloc(sizeof(Keep_Alive));
    new (buf->frame()->data<Keep_Alive>()) Keep_Alive;
    TSTP::send(buf);
}

__END_SYS

#endif
