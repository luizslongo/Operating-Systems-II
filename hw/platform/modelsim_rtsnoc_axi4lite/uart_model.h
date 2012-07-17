/*
 * uart.h
 *
 *  Created on: 30/03/2010
 *      Author: tiago
 */

#ifndef UART_MODEL_H_
#define UART_MODEL_H_

#include <systemc.h>
#include <queue>
#include <pty.h>

#define ntohs(A) ( ((A)>>8) | (((A)&0xff)<<8) )
#define htons(A) ntohs(A)
#define ntohl(A) ( ((A)>>24) | (((A)&0xff0000)>>8) | (((A)&0xff00)<<8) | ((A)<<24) )
#define htonl(A) ntohl(A)


namespace UART_MODEL {

class uart_fifo : public std::queue<unsigned int>{
	typedef std::queue<unsigned int> base;

	sc_semaphore sem;

	public:
	uart_fifo() :base(),
		sem(1){

	}

	 bool empty() { sem.wait(); bool tmp = base::empty(); sem.post(); return tmp; }

	 unsigned int size() { sem.wait(); unsigned int tmp = base::size(); sem.post(); return tmp; }

	 unsigned int& front() { sem.wait(); unsigned int &tmp = base::front(); sem.post(); return tmp;}

	 unsigned int& back() { sem.wait(); unsigned int &tmp = base::back(); sem.post(); return tmp;}

	 void push(const unsigned int& __x) { sem.wait(); base::push(__x); sem.post();}

	 void pop() { sem.wait(); base::pop(); sem.post();}

};

template<unsigned int BUFFER_SIZE>
class pseudo_terminal {
public:
	int rtnVal;
	int mpty, spty, c, dev;
	char *pName;
	char *ptyBuff;
	int buff_st;
	int buff_st_max;

	pseudo_terminal(){
		rtnVal=0;
		c=0; dev=0;
		pName=NULL;

		ptyBuff = new char[BUFFER_SIZE];
		buff_st = 0;
		buff_st_max = 0;

		rtnVal = openpty(&mpty, &spty, NULL, NULL, NULL);

		// Check if Pseudo-Term pair was created
		if(rtnVal != -1)
		{
			pName = ptsname(mpty);
			std::cout << "UART: Opened pseudo terminal at " << pName << "\n";
			//std::cout << "UART: waiting 5 sec" << std::endl; sleep(5);

		}
		else
		{
			std::cout << "UART: Failed to openOpened pseudo terminal\n";
		}

	}

	~pseudo_terminal(){
		if (ptyBuff) delete[] ptyBuff;
	}


	char terminal_read(){
		char ret_val = 0;

		if(buff_st < buff_st_max){
			ret_val = ptyBuff[buff_st];
			++buff_st;
		}
		else{
			buff_st = 0;
			buff_st_max = 0;
			while(buff_st_max <= 0) buff_st_max = read(mpty, ptyBuff, BUFFER_SIZE);

			ret_val = terminal_read();
		}

		return ret_val;
	}

	bool pty_input_available() {
		struct timeval tv;
		fd_set fds;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		FD_SET(mpty, &fds);
		select(mpty+1, &fds, NULL, NULL, &tv);
		return FD_ISSET(mpty, &fds);
	}

	bool input_available() {
		return (buff_st < buff_st_max) ? true : pty_input_available();
	}


	void terminal_write(char val){
		write(mpty, &val, 1);
	}
};

};

SC_MODULE(uart_model) {

	sc_in<sc_logic>		baudclk;

	sc_in<sc_logic> 	wr_avl;
	sc_out<sc_logic>    wr_rdy;
	sc_in<sc_lv<8> > 	wr_data;

	sc_out<sc_logic>    rd_avl;
	sc_in<sc_logic>    rd_rdy;
	sc_out<sc_lv<8> >   rd_data;

	SC_CTOR(uart_model){
		SC_THREAD(tx_thread);

		SC_THREAD(rx_thread);

		SC_THREAD(write);

		SC_THREAD(read);

	}

	void read(){

	    rd_avl = sc_logic_0;
	    rd_data = 0;

	    while(true){
	        if(!rx_fifo.empty()){
	            while(rd_rdy.read() != sc_logic_1) wait(rd_rdy.posedge_event());

	            rd_data = (unsigned int)ntohl(rx_fifo.front());
	            rx_fifo.pop();
	            rd_avl = sc_logic_1;

	            while(rd_rdy.read() != sc_logic_0) wait(rd_rdy.negedge_event());
	            rd_avl = sc_logic_0;
	        }
	    }
	}
	void write(){

	    wr_rdy = sc_logic_1;

	    while(true){

	        while(wr_avl.read() != sc_logic_1) wait(wr_avl.posedge_event());

	        tx_fifo.push((unsigned int)htonl(wr_data.read().to_uint()));
	        tx_fifo_add.notify();
	    }
	}


private:

	UART_MODEL::pseudo_terminal<64*1024*1024> pty;

	UART_MODEL::uart_fifo tx_fifo;
	sc_event tx_fifo_add;
	void tx_thread(){
	    while(true){
	        wait(tx_fifo_add);

	        while(!tx_fifo.empty()){
	            unsigned int val = tx_fifo.front();
	            tx_fifo.pop();

	            //std::cout << std::setfill('0') << std::setw(8) << std::hex << data << "\n";

	            //std::cout << (char) htonl(val);

	            pty.terminal_write((char) htonl(val));
	        }
	    }

	}

	UART_MODEL::uart_fifo rx_fifo;
	void rx_thread(){
	    while(true){
	        if(pty.input_available()){
	            char c = pty.terminal_read();
	            rx_fifo.push(htonl(0x000000FF & (unsigned int)c));
	        }
	        wait(baudclk.posedge_event());
	    }
	}
};


#endif /* UART_H_ */
