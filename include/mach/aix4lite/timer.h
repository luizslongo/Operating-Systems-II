// EPOS AIX4LITE Timer Mediator Declarations

#ifndef __aix4lite_timer_h
#define __aix4lite_timer_h

#include <cpu.h>
#include <ic.h>
#include <rtc.h>
#include <timer.h>

__BEGIN_SYS

class AIX4LITE_Timer:  public Timer_Common
{
protected:
    typedef CPU::Reg32 Count;
	typedef volatile CPU::Reg32 Timer_Reg;

    // Plasma Timer private imports, types and constants
    static const unsigned int CLOCK = Traits<AIX4LITE_Timer>::CLOCK;
	static const unsigned int FREQUENCY = Traits<AIX4LITE_Timer>::FREQUENCY;
    static const unsigned int CHANNELS = 3;

public:
  typedef int Channel;
  enum {
    SCHEDULER,
    ALARM,
	TEST
  };
 
public:
    AIX4LITE_Timer(const Hertz & frequency,
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

    ~AIX4LITE_Timer() {}

    void frequency(const Hertz & f) {
		_initial = FREQUENCY / f;
        reset();
        db<AIX4LITE_Timer>(TRC) << "AIX4LITE_Timer::frequency(freq=" << frequency()
		                             << ",cnt=" << (void*)_initial << ")\n";
    }

	Hertz frequency() {  return FREQUENCY / _initial; }

    void enable() {}

    void disable() {}

    void reset() { _current = _initial; }

    static void int_handler(unsigned int interrupt);

    static void init();

protected:
    // Plasma_Timer attributes
    Count _initial;
    volatile Count _current;
    unsigned int _channel;
    Handler * _handler;
	static Timer_Reg * _timer_reg;
    static AIX4LITE_Timer * _channels[CHANNELS];

};


// Timer used by Alarm
class Alarm_Timer: public AIX4LITE_Timer
{
  public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

  public:
    Alarm_Timer(const Handler * handler):
      AIX4LITE_Timer(FREQUENCY, handler, ALARM) {}
};


// Timer used by Thread::Scheduler
class Scheduler_Timer: public AIX4LITE_Timer
{
  private:
    typedef RTC::Microsecond Microsecond;

  public:
    Scheduler_Timer(const Microsecond & quantum, const Handler * handler): 
      AIX4LITE_Timer(1000000 / quantum, handler, SCHEDULER) {}
};


__END_SYS

#endif
