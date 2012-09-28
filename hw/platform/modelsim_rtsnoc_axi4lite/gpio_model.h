/*
 * uart.h
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#ifndef UART_H_
#define UART_H_

#include <systemc.h>
#include <sys/time.h>


SC_MODULE(gpio_model) {

	sc_in<sc_uint<32> > gpio_i;


	SC_CTOR(gpio_model)
	{

		real_time_start = get_real_time_ms();

		for (int i = 0; i < 10; ++i) {
		    std::cout << "## INFO: GPIO_OUT\n";
        }

		SC_METHOD(print_time);
		sensitive << gpio_i;

	}


private:
	long int real_time_start;

	void print_time();

	long int get_real_time_ms(){
	    struct timeval now;
	    gettimeofday(&now, NULL);
	    long int ms_now = now.tv_sec * 1000 + now.tv_usec / 1000;
	    return ms_now;
	}
};

#endif /* UART_H_ */
