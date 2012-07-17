/*
 * common.h
 *
 *  Created on: Jul 17, 2012
 *      Author: tiago
 */

#ifndef COMMON___H
#define COMMON___H

#include <systemc.h>

template<int N_PORTS>
SC_MODULE(bool_to_logic) {

    sc_in<bool> inputs[N_PORTS];
    sc_out<sc_logic> outputs[N_PORTS];

    SC_CTOR(bool_to_logic){
        SC_METHOD(convert);
        for (int i = 0; i < N_PORTS; ++i) sensitive << inputs[i];
    }

    void convert(){
        for (int i = 0; i < N_PORTS; ++i) {
            outputs[i] = inputs[i] ? sc_logic_1 : sc_logic_0;
        }
    }

};

template<int N_PORTS, int SIZE>
SC_MODULE(lv_to_uint) {

    sc_in<sc_lv<SIZE> > inputs[N_PORTS];
    sc_out<sc_uint<SIZE> > outputs[N_PORTS];

    SC_CTOR(lv_to_uint){
        SC_METHOD(convert);
        for (int i = 0; i < N_PORTS; ++i) sensitive << inputs[i];
    }

    void convert(){
        for (int i = 0; i < N_PORTS; ++i) {
            outputs[i] = inputs[i].read().to_uint();
        }
    }

};

template<int N_PORTS, int SIZE>
SC_MODULE(uint_to_lv) {

    sc_in<sc_uint<SIZE> > inputs[N_PORTS];
    sc_out<sc_lv<SIZE> > outputs[N_PORTS];

    SC_CTOR(uint_to_lv){
        SC_METHOD(convert);
        for (int i = 0; i < N_PORTS; ++i) sensitive << inputs[i];
    }

    void convert(){
        for (int i = 0; i < N_PORTS; ++i) {
            outputs[i] = inputs[i].read();
        }
    }

};

#endif
