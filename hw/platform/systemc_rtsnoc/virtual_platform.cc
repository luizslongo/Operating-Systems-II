/*
 * main.cpp
 *
 *  Created on: 28/02/2010
 *      Author: tiago
 */

#include <systemc.h>
#include <iostream>
#include <ctime>
#include <epos_soc.h>

SC_MODULE(virtual_platform) {

	enum{
		CLK = 50 //MHz
	};

	epos_soc	soc;

	sc_signal<bool> rst;
	sc_time	 clk_period;
	sc_clock clk;

	sc_signal<sc_uint<32> > gpio_in;
	sc_signal<sc_uint<32> > gpio_out;

	sc_signal<bool> ext_int[8];

	std::time_t	real_time_start;

	SC_CTOR(virtual_platform)
		:soc("epos_soc"),
		 rst("rst"),
		 clk_period(1000/CLK, SC_NS),
		 clk("clk", clk_period)
	{

		soc.clk(clk);
		soc.rst(rst);
		soc.gpio_in(gpio_in);
		soc.gpio_out(gpio_out);
		for (int i = 0; i < 8; ++i) soc.ext_int[i](ext_int[i]);

		SC_THREAD(start_plat);

		SC_THREAD(print_time);

		rst = true;
		for (int i = 0; i < 8; ++i) ext_int[i] = false;
		//gpio_in = 0xFFFFFFFF;

#ifdef dump_signals_vcd
		dump_signals();
#endif
	}

	void start_plat();

	void print_time();

	void dump_signals();

};

void virtual_platform::dump_signals(){
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

}

void virtual_platform::start_plat(){

	rst = true;


	bool boot_ok = soc.load_bootloader("loader.txt");

	if(boot_ok) {

		std::cout << "Waiting some cycles\n";
		for (int i = 0; i < 10; ++i) {
			sc_module::wait(clk.posedge_event());
		}

		std::cout << "Setting reset to false\n\n\n";
		rst = false;

	} else {
		std::cout << "ERROR: CAN'T LOAD BOOT LOADER";
		sc_stop();
	}
}

void virtual_platform::print_time(){
	while(true){
		sc_time wait_time(100,SC_MS);
		sc_module::wait(wait_time);

		std::time_t now;
		std::time(&now);
		std::cout << "## INFO: Simulation time: " << sc_time_stamp().to_seconds()
				  << "s | Real time: " << std::difftime(now,real_time_start) << "s "
				  << " | TSC: " << soc.get_tsc() << "\n";
	}
}


int sc_main (int argc, char *argv[]){


	virtual_platform plat("virtual_platform");

	std::time(&(plat.real_time_start));
	sc_start();

	return 0;
}
