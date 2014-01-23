#include <alarm.h>
#include <gpio_pin.h>

__USING_SYS

// TODO: Peterson, faz uma classe decente :)
class AC_Control {
public:
	AC_Control() : _pin(13), data0_mask(0) {}
	~AC_Control() {}

	typedef Machine::IO IO;

	enum status {
		keep_on = 0x24, turn_off = 0x4
	};

	enum temperatures {
		plus = 0xE8, minus = 0xE4, neither = 0xE0
	};

	enum commands {
		command_on = 0x52,
		command_off = 0x62,
		command_plus = 0x5A,
		command_minus = 0x56
	};

	void set_pin() {
		_pin.put(1);
	}

	void clear_pin() {
		_pin.put(0);
	}

	void delay10u(volatile unsigned int count) {
		volatile unsigned int inner_count;
		for (volatile unsigned int i = 0; i < count; i++) {
			inner_count = 18;
			while (inner_count--) {
			};
		}
	}

	void fulfil_set_command(int iterations, int calibration) {

		for (int i = 0; i < iterations; i++) {
			set_pin();
			delay10u(calibration);
			clear_pin();
			delay10u(calibration);
		}

	}

	void fulfil_clear_command(int iterations, float calibration) {

		for (int i = 0; i < iterations; i++) {
			clear_pin();
			delay10u(calibration);
			set_pin();
			delay10u(calibration);
		}

	}

	void put_L(int calibration = 0) {
		fulfil_clear_command(200, 1.2);
		set_pin();
		delay10u(192 + 7);
	}

	void put_S(int calibration = 0) {
		fulfil_set_command(13, 1.2);
		set_pin();
		delay10u(840 + 18);
		clear_pin();
	}

	void put_0(int calibration = 0) {
		fulfil_set_command(13, 1.2);
		delay10u(53 + calibration);
	}

	void put_1(int calibration = 0) {
		fulfil_set_command(13, 1.2);
		delay10u(137 + 7);
	}

	void put_either(char either) {
		if (either)
			put_1();
		else
			put_0();
	}

	void put_c(char c) {
		for (int i = 7; i >= 0; i--) {
			put_either(c & (1 << i));
		}
	}

	void send_command(char on_off, char temperature, char command) {
		put_L();
		put_c(0xC4);
		put_c(0xD3);
		put_c(0x64);
		put_c(0x80);
		put_c(0x0);
		put_c(on_off);
		put_c(0x10);
		put_c(temperature);
		put_c(0x40);
		put_c(0x0);
		put_c(0x0);
		put_c(0x0);
		put_c(0x0);
		put_c(command);
		put_S();
	}

	void turn_ac_off() {
		send_command(turn_off, neither, command_off);
	}

	void turn_ac_on() {
		send_command(keep_on, neither, command_on);
	}

	void increase_temperature() {
		send_command(keep_on, plus, command_plus);
	}

	void decrease_temperature() {
		send_command(keep_on, minus, command_minus);
	}

private:
	GPIO_Pin _pin;
	OStream cout;

	unsigned int data0_mask;
};
