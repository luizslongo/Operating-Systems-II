// EPOS-- PC RTC Mediator Implementation

#include <mach/pc/rtc.h>

__BEGIN_SYS

PC_RTC::Seconds PC_RTC::get()
{
    unsigned int Y, M, D, h, m, s;

    do { // Get time BINARY (not BCD) components
	s = reg(SECONDS);
	m = reg(MINUTES);
	h = reg(HOURS);
	D = reg(DAY);
	M = reg(MONTH);
	Y = reg(YEAR);
    } while(s != reg(SECONDS)); // RTC update in between? read again!
    if ((Y += 1900) < Traits::EPOCH_YEAR)
	Y += 100;

    return date2offset(Traits::EPOCH_DAYS, Y, M, D, h, m, s);
}

void PC_RTC::set(const PC_RTC::Seconds & time)
{
    db<PC_RTC>(TRC) << "PC_RTC::write(time= " << time << ")\n";

    unsigned int Y, M, D, h, m, s;

    offset2date(time, Traits::EPOCH_DAYS, &Y, &M, &D, &h, &m, &s);

    reg(YEAR, Y);
    reg(MONTH, M);
    reg(DAY, D);
    reg(HOURS, h);
    reg(MINUTES, m);
    reg(SECONDS, s);
}

__END_SYS
