/*
 * uart.cpp
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#include "gpio_model.h"

void gpio_model::print_time(){
    std::cout << "## INFO: GPIO_OUT = " << gpio_i.read().to_string(SC_HEX_US) << "\n";
    std::cout << "##       Simulation time: " << sc_time_stamp().to_seconds()
              << "s | Real time: " << (get_real_time_ms() - real_time_start)/1000.0 << "s" << std::endl;
}

SC_MODULE_EXPORT(gpio_model);


