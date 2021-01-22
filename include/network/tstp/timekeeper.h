// EPOS Trustful SpaceTime Protocol Timekeeper Declarations

#ifndef __tstp_timekeeper_h
#define __tstp_timekeeper_h

#include <system/config.h>

#ifdef __tstp__

#include <machine/nic.h>

__BEGIN_SYS

class TSTP::Timekeeper: private SmartData, private Data_Observer<Buffer>
{
    friend class TSTP;

private:
    static const unsigned int MAX_DRIFT = 500000; // us
#ifdef __ieee802_15_4__
    static const unsigned int NIC_TIMER_INTERRUPT_DELAY = IEEE802_15_4::SHR_SIZE * 1000000 / IEEE802_15_4::BYTE_RATE; // us FIXME: this has to come from NIC
#else
    static const unsigned int NIC_TIMER_INTERRUPT_DELAY = 0; // us
#endif

public:
    // Epoch Control Message
    class Epoch: public Control
    {
    public:
        Epoch(const Region & r, const Time & t = Timekeeper::reference(), const Global_Space & c = Locator::reference())
        : Control(r, 0, 0, EPOCH), _reference(t), _coordinates(c) { }

        Region destination() const { return Region(_origin, _radius, _t1); }
        const Time epoch() const { return _reference; }
        const Global_Space & coordinates() const { return _coordinates; }

        friend Debug & operator<<(Debug & db, const Epoch & e) {
            db << reinterpret_cast<const Control &>(e) << ",d=" << e.destination() << ",e=" << e._reference << ",c=" << e._coordinates;
            return db;
        }

    private:
        Time _reference;
        Global_Space _coordinates;
    } __attribute__((packed));

    // Keep Alive Control Message
    class Keep_Alive: public Control
    {
    public:
        Keep_Alive(): Control(Spacetime(here(), now()), 0, 0, KEEP_ALIVE) {}

        friend Debug & operator<<(Debug & db, const Keep_Alive & k) {
            db << reinterpret_cast<const Control &>(k);
            return db;
        }
    } __attribute__((packed));

public:
    Timekeeper();
    ~Timekeeper();

    static Time now() { return ts2us(time_stamp()); }
    static bool synchronized() { return !sync_required(); }
    static Time reference() { return _reference; }

    static Time absolute(const Time & t) { return _reference + t; }
    static Time relative(const Time & t) { return t - _reference; }

private:
    void update(Data_Observed<Buffer> * obs, Buffer * buf);

    static void reference(const Time & t) { _reference = t; }

    static void marshal(Buffer * buf);

    static Time_Stamp time_stamp() { return _nic->time_stamp(); }

    static Time_Stamp sync_period() {
        long long tmp = LARGER<PPM>::Result(timer_accuracy()) * LARGER<Hertz>::Result(timer_frequency());
        tmp /= 1000000000LL; // us
        tmp = (static_cast<long long>(MAX_DRIFT) * 1000000) / tmp;
        return static_cast<Time_Stamp>(tmp);
    }
    static bool sync_required() { return (_next_sync == 0) || (time_stamp() >= (_next_sync - sync_period() / 2)); }
    static void keep_alive();

private:
    static Time _reference;
    static volatile Time_Stamp _next_sync;
    static Function_Handler * _life_keeper_handler;
    static Alarm * _life_keeper;
};

__END_SYS

#endif

#endif
