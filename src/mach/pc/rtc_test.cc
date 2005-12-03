// EPOS-- PC_RTC Test Program

#include <utility/ostream.h>
#include <display.h>
#include <rtc.h>

const unsigned int TEST_DURATION = 20; // s

__USING_SYS

int main()
{
    OStream cout;

    cout << "PC_RTC test\n";

    PC_RTC rtc;
    RTC::Second t0 = rtc.get();
    Display disp;
    unsigned int last_second = rtc.seconds();
    for(int i = 0; i < 2; i++) {
	while(rtc.get() < t0 + TEST_DURATION) {
	    if(rtc.seconds() != last_second) {
		last_second = rtc.seconds();
		disp.position(20, 30);
		cout << rtc.day() << '/'
		     << rtc.month() << '/'
		     << rtc.year() << ' ';
		disp.position(20, 40);
		cout << rtc.hours() << ':'
		     << rtc.minutes() << ':'
		     << rtc.seconds() << "    ";
	    }
	}
	cout << "\n\nSetting the time to its previous value (set(get()).\n";
// 	rtc.set(rtc.get());
    }

    return 0;
}
