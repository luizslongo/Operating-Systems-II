/*
 * main.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include <systemc.h>
#include "common.h"
#include "axi4_reset_control.h"
#include "axi4lite_old.h"
#include "uart_rx_wrapper.h"
#include "uart_model.h"

SC_MODULE(virtual_platform) {

	enum{
		CLK = 50 //MHz
	};
	static const char* const FREQ_S[];

	axi4lite_old	soc;
	axi4_reset_control rst_ctr;
	uart_rx_wrapper uart_rx;
	uart_model uart_mdl;

	bool_to_logic<1> bool2logic;

	sc_time     clk_period;
	sc_clock clk;

	sc_signal<bool> rst_bnt;
	sc_signal<sc_logic> rst_bnt_logic;
	sc_signal<sc_logic> rst;
	sc_signal<sc_logic> clk_logic;

	sc_signal<sc_logic> soc_uart_tx;
	sc_signal<sc_logic> soc_uart_rx;
	sc_signal<sc_logic> soc_uart_baud;

	sc_signal<sc_lv<32> > gpio_in;
	sc_signal<sc_lv<32> > gpio_out;

	sc_signal<sc_lv<8> > ext_int;


	sc_signal<sc_logic>    wr_avl;
	sc_signal<sc_logic>    wr_rdy;
	sc_signal<sc_lv<8> >   wr_data;

	sc_signal<sc_logic>    rd_avl;
	sc_signal<sc_logic>    rd_rdy;
	sc_signal<sc_lv<8> >   rd_data;


	std::time_t	real_time_start;

	SC_CTOR(virtual_platform)
		:soc("soc", "axi4lite_old", 1, (const char**)FREQ_S),
		 rst_ctr("rst_ctr", "axi4_reset_control"),
		 uart_rx("uart_rx", "uart_rx_wrapper"),
		 uart_mdl("uart_model"),
		 bool2logic("bool2logic"),
		 clk_period(1000/CLK, SC_NS),
		 clk("clk", clk_period)
	{

	    bool2logic.inputs[0](clk);
	    bool2logic.outputs[0](clk_logic);
	    bool2logic.inputs[1](rst_bnt);
	    bool2logic.outputs[1](rst_bnt_logic);


	    soc.clk_i(clk_logic);
		soc.reset_i(rst);
		soc.uart_tx_o(soc_uart_tx);
		soc.uart_rx_i(soc_uart_rx);
		soc.uart_baud_o(soc_uart_baud);
		soc.gpio_i(gpio_in);
		soc.gpio_o(gpio_out);
		soc.ext_int_i(ext_int);

		rst_ctr.clk_i(clk_logic);
		rst_ctr.ext_reset_i(rst_bnt_logic);
		rst_ctr.axi_reset_o(rst);

		uart_mdl.baudclk(soc_uart_baud);
		uart_mdl.rd_avl(rd_avl);
		uart_mdl.rd_rdy(rd_rdy);
		uart_mdl.rd_data(rd_data);
		uart_mdl.wr_avl(wr_avl);
		uart_mdl.wr_rdy(wr_rdy);
		uart_mdl.wr_data(wr_data);

		uart_rx.baudclk(soc_uart_baud);
		uart_rx.rxd(soc_uart_tx);
		uart_rx.wr_avl(wr_avl);
		uart_rx.wr_rdy(wr_rdy);
		uart_rx.wr_data(wr_data);

		SC_THREAD(start_plat);

		SC_THREAD(print_time);


		rst_bnt = true;
		ext_int = 0;


		//dump_signals();

	}

	void start_plat(){

	    wait(rst.negedge_event());
	    std::cout << "Reset released\n\n\n";

	}

	void print_time(){
	    while(true){
	        sc_time wait_time(100,SC_MS);
	        sc_module::wait(wait_time);

	        std::time_t now;
	        std::time(&now);
	        std::cout << "## INFO: Simulation time: " << sc_time_stamp().to_seconds()
	                  << "s | Real time: " << std::difftime(now,real_time_start) << "s\n";
	    }
	}

	/*void dump_signals(){
	    sc_trace_file *fp;

	    fp=sc_create_vcd_trace_file("virtual_platform");

	    sc_trace(fp, clk, "clk");
	    sc_trace(fp, rst, "rst");

	    sc_trace(fp, soc.node_proc_io.irq, "pic_irq");
	    for (int i = 0; i < 32; ++i) {
	        std::ostringstream data;
	        data << "irq(" << i << ")";
	        sc_trace(fp, soc.node_proc_io.m_pic.irq_in[i], data.str());

	    }
	    sc_trace(fp, gpio_in, "gpio_in");
	    sc_trace(fp, gpio_out, "gpio_out");
	    for (int i = 0; i < epos_soc::ROUTER_N_PORTS; ++i) {
	        std::ostringstream data0,data1,data2,data3,data4,data5;
	        data0 << "noc_din(" << i << ")";
	        data1 << "noc_dout(" << i << ")";
	        data2 << "noc_nd(" << i << ")";
	        data3 << "noc_rd(" << i << ")";
	        data4 << "noc_wait(" << i << ")";
	        data5 << "noc_wr(" << i << ")";
	        sc_trace(fp, soc.noc_din[i], data0.str());
	        sc_trace(fp, soc.noc_dout[i], data1.str());
	        sc_trace(fp, soc.noc_nd[i], data2.str());
	        sc_trace(fp, soc.noc_rd[i], data3.str());
	        sc_trace(fp, soc.noc_wait[i], data4.str());
	        sc_trace(fp, soc.noc_wr[i], data5.str());
	    }

	#ifdef dump_internal
	    sc_trace(fp,cpu.fet->pc,"cpu_fetch_pc");
	    sc_trace(fp,cpu.fet->pc_data->epc,"cpu_fetch_epc");
	    sc_trace(fp,cpu.exe->exceptionId,"cpu_exe_excep");
	    sc_trace(fp,cpu.exe->epc,"cpu_exe_epc");
	    sc_trace(fp,cpu.exe->status,"cpu_exe_status");
	#endif

	}*/

};

const char* const virtual_platform::FREQ_S[] = {"50000000"};

SC_MODULE_EXPORT(virtual_platform);


