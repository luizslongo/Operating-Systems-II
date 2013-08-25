// EPOSMote2 Startup Board Mediators

#ifndef __emote2_startup_h
#define __emote2_startup_h

#include <cpu.h>
#include <machine.h>
#include <sensor.h>
#include <uart.h>
#include <gpio_pin.h>

__BEGIN_SYS

class EMote2_Startup
{
public:
    EMote2_Startup()
	: _led1_r((Traits<MC13224V>::emote_version == 0x2f) ? 10 : 23),
	  _led1_g((Traits<MC13224V>::emote_version == 0x2f) ?  9 : 24),
	  _led1_b((Traits<MC13224V>::emote_version == 0x2f) ? 11 : 25),
	  _led2  ((Traits<MC13224V>::emote_version == 0x2f) ?  8 : 26),
	  _led3  ((Traits<MC13224V>::emote_version == 0x2f) ? 23 : 27),
	  _but1  ((Traits<MC13224V>::emote_version == 0x2f) ? 24 : 11),
	  _but2  ((Traits<MC13224V>::emote_version == 0x2f) ? 22 : 10)
	{
    	reset();
	}

    ~EMote2_Startup() {}

    void reset() {
//    	_termistor.power(OFF);
//        _accel.power(OFF);
//        _uart.power(OFF);
        clear_green();
        clear_red();
        clear_rgb();
    }

    // Temperature Sensor
    int temperature() { return _termistor.sample(); }
    Temperature_Sensor & temperature_sensor() { return _termistor; }

    // TODO: Accelerometer
    int x_acceleration() { return _accel.sample_x(); }
    int y_acceleration() { return _accel.sample_y(); }
    int z_acceleration() { return _accel.sample_z(); }
    Accelerometer & accelerometer() { return _accel; }

    // Host/USB UART
    char get_char() { return _uart.get(); }
    void put_char(char c) { _uart.put(c); }
    UART & uart() { return _uart; }

    // LED1 (RGB)
    void set_rgb(char red, char green, char blue)
    {
        //TODO: use PWMs
        if(red) _led1_r.set();
        else _led1_r.clear();
        if(green) _led1_g.set();
        else _led1_g.clear();
        if(blue) _led1_b.set();
        else _led1_b.clear();
    }
    void clear_rgb()
    {
        _led1_r.clear();
        _led1_g.clear();
        _led1_b.clear();
    }

    // LED2
    void set_green() { _led2.set(); }
    void clear_green() { _led2.clear(); }

    // LED3
    void set_red() { _led3.set(); }
    void clear_red() { _led3.clear(); }

    // BUT1
    // TODO: interrupts
    bool get_button1() { return _but1.get(); }

    // BUT2
    // TODO: interrupts
    bool get_button2() { return _but2.get(); }

private:
    Temperature_Sensor _termistor;
    Accelerometer _accel;
    UART _uart;
    GPIO_Pin _led1_r;
    GPIO_Pin _led1_g;
    GPIO_Pin _led1_b;
    GPIO_Pin _led2;
    GPIO_Pin _led3;
    GPIO_Pin _but1;
    GPIO_Pin _but2;
};

__END_SYS

#endif
