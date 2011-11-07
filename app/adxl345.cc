#include <machine.h>
#include <alarm.h>

__USING_SYS

int main() {
	OStream cout;
	ADXL345 accel;
	int x, y, z;

	cout << "ADXL345 accelerometer test\n";

	accel.enable_x();
	accel.enable_y();
	accel.enable_z();

	while(1) {
		x = accel.sample_x();
		y = accel.sample_y();
		z = accel.sample_z();

		cout << "x = " << x << ", ";
		cout << "y = " << y << ", ";
		cout << "z = " << z << "\n";

		Alarm::delay(100);
	}

	return 0;
}
