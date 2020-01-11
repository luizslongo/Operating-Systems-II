#include <machine.h>
#include <transducer.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "Transducer test" << endl;

    cout << "This is an accelerometer test." << endl;

    const unsigned int FREQUENCY = 120; // Hz
    const unsigned int PERIOD = ((1/FREQUENCY)*1000000);

    Accelerometer ax(0), ay(1), az(2);
    Gyroscope gx(0), gy(1), gz(2);

    while(true) {
        cout << "Acceleration     -> (X=" << ax.sense() << ", Y=" << ay.sense() << ",Z=" << az.sense() <<") g" << endl;
        cout << "Angular Velocity -> (X=" << gx.sense() << ", Y=" << gy.sense() << ",Z=" << gz.sense() <<") g" << endl;
        Alarm::delay(PERIOD);
    }

    return 0;
}
