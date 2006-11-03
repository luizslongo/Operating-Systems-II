// EPOS-- Power Management Utility Implementation

#include <utility/power_manager.h>

#include <cpu.h>
#include <uart.h>
#include <adc.h>

__BEGIN_SYS

//AVR8

template<> Power_Manager<AVR8,false,true> * Power_Manager<AVR8,false,true>::_head = 0;
template<> Power_Manager<AVR8,true,true> * Power_Manager<AVR8,true,true>::_head = 0;
template<> unsigned int Power_Manager<AVR8,false,true>::_instances_counter = 0;
template<> unsigned int Power_Manager<AVR8,true,true>::_instances_counter = 0;

template<>
void Power_Manager<AVR8,Traits<AVR8>::shared,Traits<AVR8>::instances>::power(int mode) {

    if(Traits<AVR8>::shared) {
        if(!check_shared(mode)) return;
    }

    _prev_op_mode = _op_mode;
    _op_mode = mode;

    switch(mode) {
    case Traits<AVR8>::E0:
        sleep(mode);
	break;
    case Traits<AVR8>::E1:
        sleep(mode);
	break;
    case Traits<AVR8>::E2:
        sleep(mode);
	break;
    case Traits<AVR8>::E3:
        sleep(mode);
	break;
    }
}


//ATMega128_UART

template<> Power_Manager<ATMega128_UART,false,true> * Power_Manager<ATMega128_UART,false,true>::_head = 0;
template<> Power_Manager<ATMega128_UART,true,true> * Power_Manager<ATMega128_UART,true,true>::_head = 0;
template<> unsigned int Power_Manager<ATMega128_UART,false,true>::_instances_counter = 0;
template<> unsigned int Power_Manager<ATMega128_UART,true,true>::_instances_counter = 0;

template<>
void Power_Manager<ATMega128_UART,Traits<ATMega128_UART>::shared,Traits<ATMega128_UART>::instances>::power(int mode) {

    if(Traits<ATMega128_UART>::shared) {
        if(!check_shared(mode)) return;
    }

    _prev_op_mode = _op_mode;
    _op_mode = mode;
    switch(mode) {
    case Traits<ATMega128_UART>::E0:
        this->ucsrb(this->ucsrb() | (1 << ATMega128_UART::TXEN) | (1 << ATMega128_UART::RXEN));
	break;
    case Traits<ATMega128_UART>::E1:
        this->ATMega128_UART::ucsrb(this->ATMega128_UART::ucsrb() & ~(1 << UART::RXEN));
	//When using the FIFO buffer, it has to be flushed at this
	//point to avoid loss of data.
	this->ucsrb(this->ucsrb() | (1 << UART::TXEN));
	break;
    case Traits<ATMega128_UART>::E2:
        this->ucsrb(this->ucsrb() & ~(1 << UART::TXEN));
	this->ucsrb(this->ucsrb() | (1 << UART::RXEN));
	break;
    case Traits<ATMega128_UART>::E3:
        this->ucsrb(this->ucsrb() & ~((1 << UART::TXEN) | (1 << UART::RXEN)));
	break;
    }
}

template<>
int Power_Manager<ATMega128_UART,Traits<ATMega128_UART>::shared,Traits<ATMega128_UART>::instances>::get() {
    if(!((_op_mode == Traits<ATMega128_UART>::RECEIVE_ONLY) || (_op_mode == Traits<ATMega128_UART>::FULL))) {
        if((_prev_op_mode == Traits<ATMega128_UART>::RECEIVE_ONLY) || (_prev_op_mode == Traits<ATMega128_UART>::FULL))
	    power(_prev_op_mode);
	else
	    power(Traits<ATMega128_UART>::FULL);
    }

    return ATMega128_UART::get();
}

template<>
void Power_Manager<ATMega128_UART,Traits<ATMega128_UART>::shared,Traits<ATMega128_UART>::instances>::put(char c) {

    if(!((_op_mode == Traits<ATMega128_UART>::SEND_ONLY) || (_op_mode == Traits<ATMega128_UART>::FULL))) {
        if((_prev_op_mode == Traits<ATMega128_UART>::SEND_ONLY) || (_prev_op_mode == Traits<ATMega128_UART>::FULL))
	    power(_prev_op_mode);
  	else
	    power(Traits<ATMega128_UART>::FULL);
    }

    ATMega128_UART::put(c);
}

//ATMega128_ADC

template<> Power_Manager<ATMega128_ADC,false,true> * Power_Manager<ATMega128_ADC,false,true>::_head = 0;
template<> Power_Manager<ATMega128_ADC,true,true> * Power_Manager<ATMega128_ADC,true,true>::_head = 0;
template<> unsigned int Power_Manager<ATMega128_ADC,false,true>::_instances_counter = 0;
template<> unsigned int Power_Manager<ATMega128_ADC,true,true>::_instances_counter = 0;

template<>
void Power_Manager<ATMega128_ADC,Traits<ATMega128_ADC>::shared,Traits<ATMega128_ADC>::instances>::power(int mode) {

    if(Traits<ATMega128_ADC>::shared) {
        if(!check_shared(mode)) return;
    }

    _prev_op_mode = _op_mode;
    _op_mode = mode;
    switch(mode) {
    case Traits<ATMega128_ADC>::E0:
        this->enable();
	break;
    case Traits<ATMega128_ADC>::E3:
        this->disable();
	break;
    }
}

template<>
int Power_Manager<ATMega128_ADC,Traits<ATMega128_ADC>::shared,Traits<ATMega128_ADC>::instances>::sample() {
    if(!(_op_mode == Traits<ATMega128_ADC>::FULL))
        power(Traits<ATMega128_ADC>::FULL);

    return ATMega128_ADC::sample();
}

template<>
int Power_Manager<ATMega128_ADC,Traits<ATMega128_ADC>::shared,Traits<ATMega128_ADC>::instances>::get() {
    if(!(_op_mode == Traits<ATMega128_ADC>::FULL))
        power(Traits<ATMega128_ADC>::FULL);

    return ATMega128_ADC::get();
}


__END_SYS
