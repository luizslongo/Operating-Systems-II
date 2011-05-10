#include <machine.h>

__USING_SYS


#include <ieee1451_tim_temperature_sensor.h>

int tim()
{
    kout << "+++++ Iniciando wtim +++++\n";

    IEEE1451TemperatureSensor sensor(true, true);
    sensor.execute();
}


/*#include <ieee1451_ncap_application.h>

int ncap()
{
    kout << "+++++ Iniciando ncap +++++\n";

    NCAPApplication::getInstance();

    Thread::self()->suspend();
    //while(1);
}*/


int main()
{
    tim();
    //ncap();
}
