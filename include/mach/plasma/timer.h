// EPOS PLASMA Timer Mediator Declarations

#ifndef __plasma_timer_h
#define __plasma_timer_h

#include <cpu.h>
#include <ic.h>
#include <rtc.h>
#include <timer.h>

__BEGIN_SYS

class PLASMA_Timer:  public Timer_Common
{
protected:
    typedef CPU::Reg32 Count;

    // Plasma Timer private imports, types and constants
    static const unsigned int CLOCK = Traits<PLASMA>::CLOCK;
    static const unsigned int FREQUENCY = Traits<PLASMA_Timer>::FREQUENCY;
    static const unsigned int CHANNELS = 2;

public:
  typedef int Channel;
  enum {
    SCHEDULER,
    ALARM
  };
 
public:
    PLASMA_Timer(int unit = 0) {} // actual initialization is up to init
    
    PLASMA_Timer(const Hertz & frequency,
                 const Handler * handler,
                 const Channel & channel):
        _initial(FREQUENCY / frequency), _current(_initial), 
	    _channel(channel), _handler(handler)
    {
      db<Timer>(TRC) << "Timer(f=" << frequency
        << ",h=" << reinterpret_cast<void*>(handler)
        << ",ch=" << channel 
        << ") => {count=" << _initial << "}\n";

      if(_initial && !_channels[channel]) 
         _channels[channel] = this;
      else
         db<Timer>(ERR) << "Timer not installed!\n";
    }

    ~PLASMA_Timer() {}

    void frequency(const Hertz & f) {
        _count = freq2cnt(f);
        reset();
        db<PLASMA_Timer>(TRC) << "PLASMA_Timer::frequency(freq=" << frequency()
		                             << ",cnt=" << (void*)_count << ")\n";
    }

    Hertz frequency() {  return cnt2freq(_count); }

    void enable() {}

    void disable() {}

    void reset() {}

    static void int_handler(unsigned int interrupt); //will be overriden by alarm

    static void init();

private:
    static Hertz cnt2freq(unsigned int c) { return CLOCK / c; }
    static unsigned int freq2cnt(const Hertz & f) { return CLOCK / f; }
	
protected:
    // Plasma_Timer attributes
    Count _initial;
    volatile Count _current;
    static Count _count;
    unsigned int _channel;
    Handler * _handler;
    static PLASMA_Timer * _channels[CHANNELS];

};



// Timer used by Alarm
class Alarm_Timer: public PLASMA_Timer
{
  public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

  public:
    Alarm_Timer(const Handler * handler):
      PLASMA_Timer(FREQUENCY, handler, ALARM) {}
};


// Timer used by Thread::Scheduler
class Scheduler_Timer: public PLASMA_Timer
{
  private:
    typedef RTC::Microsecond Microsecond;

  public:
    Scheduler_Timer(const Microsecond & quantum, const Handler * handler): 
      PLASMA_Timer(1000000 / quantum, handler, SCHEDULER) {}
};


__END_SYS

#endif
