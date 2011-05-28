#include <machine.h>
#include <alarm.h>

__USING_SYS


#include <ieee1451_tim_temperature_sensor.h>
//#include <ieee1451_tim_audio_sensor.h>
#include <ieee1451_tim.h>

int tim()
{
    //Alarm::delay(3000000);
    kout << "+++++ Starting wtim +++++\n";

    IP *ip = IP::instance();
    ip->set_address(IP::Address(10, 0, 0, 111));
    ip->set_gateway(IP::Address(10, 0, 0, 1));
    ip->set_netmask(IP::Address(255, 255, 255, 0));

    IEEE1451_Dot5_TIM *tim = IEEE1451_Dot5_TIM::get_instance();
    tim->set_ncap_address(IP::Address(10, 0, 0, 110));

    IEEE1451_Temperature_Sensor sensor(true, true);
    //IEEE1451_Audio_Sensor sensor(true, true);
    sensor.execute();
    return 0;
}


/*#include <ieee1451_ncap_application.h>

int ncap()
{
    //Alarm::delay(3000000);
    kout << "+++++ Starting ncap +++++\n";

    IP *ip = IP::instance();
    ip->set_address(IP::Address(10, 0, 0, 110));
    ip->set_gateway(IP::Address(10, 0, 0, 1));
    ip->set_netmask(IP::Address(255, 255, 255, 0));

    IEEE1451_NCAP_Application::get_instance();

    Thread::self()->suspend();
    //while(1);
    return 0;
}*/


int main()
{
    tim();
    //ncap();
}
