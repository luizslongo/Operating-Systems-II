#include <system/config.h>
#include <mach/avrmcu/avrmcu.h>

__USING_SYS

extern "C" unsigned __bss_end;

AVR8::IO_Registers * AVR8::io = (AVR8::IO_Registers *)0x20;
AVRMCU::Handler __iv[Traits<AVRMCU>::INT_VEC_SIZE];
AVRMCU::Handler * AVRMCU::interrupt_vector = __iv;


extern "C" {

void __setup() {

    System_Info * __si;

    __si = reinterpret_cast<System_Info *>(Memory_Map<Machine>::SYS_INFO);

    //(*(unsigned char *)(0x37)) = 0xff;
    //(*(unsigned char *)(0x38)) = 0xbb;
    

     //__si->bm.mem_base;         // Memory base 
     //__si->bm.mem_size;     // Memory size (in bytes)
     //__si->bm.cpu_type;              // Processor type 
     //__si->bm.cpu_clock;             // Processor clock frequency in Hz 
     //__si->bm.n_threads;             // Max number of threads 
     //__si->bm.n_tasks;               // Max number of tasks 
     //__si->bm.host_id;    // The local host id (-1 => RARP) 
     //__si->bm.n_nodes;    // Number of nodes in SAN 
     //__si->bm.img_size;              // Boot image size in bytes 
     __si->bm.setup_off	= -1;             // SETUP offset in the boot image 
     //__si->bm.system_off;            // OS offset in the boot image 
     //__si->bm.loader_off;            // LOADER 	offset in the boot image 
     //__si->bm.app_off;               // APPs offset in the boot image 
  

    

     //__si->pmm.app_lo;     // Application memory's lowest address
     //__si->pmm.app_hi;        // Application memory's highest address
     //__si->pmm.int_vec;       // Interrupt Vector
     //__si->pmm.sys_pt;        // System Page Table
     //__si->pmm.sys_pd;        // System Page Directory
     __si->pmm.sys_info		= (unsigned)&__si;	// System Info
     //__si->pmm.phy_mem_pts;   		// Page tables to map the whole physical memory
     //__si->pmm.io_mem_pts;    		// Page tables to map the IO address space 
     __si->pmm.sys_code		= 0x00;      	// OS Code Segment
     __si->pmm.sys_data      	= 0x00;		// OS Data Segment
     //__si->pmm.sys_stack;     // OS Stack Segment*/
	
    __si->pmm.free 		= (unsigned)&__bss_end;          // Free memory base
    __si->pmm.free_size 	= 128; 	// Free memory size (in frames)
    /*__si->pmm.mach1;         // Machine specific entries
    __si->pmm.mach2;
    __si->pmm.mach3;*/


    /*__si->lmm.base = 0x60;        // Lowest valid logical address
    __si->lmm.top = 0x45f;         // Highest valid logical address
    __si->lmm.app_lo;      // Application memory lowest address
    __si->lmm.app_entry;   // First application's entry point
    __si->lmm.app_code;    // First application's code base address
    __si->lmm.app_data;    // First application's data base address
    __si->lmm.app_hi;      // Application memory highest address
    __si->lmm.phy_mem;     // Whole physical memory (contiguous)
    __si->lmm.io_mem;      // IO address space */
    
    __si->lmm.int_vec 		= (unsigned)__iv;     // Interrupt Vector
    
    
    /*
    __si->lmm.sys_pt;      // System Page Table
    __si->lmm.sys_pd;      // System Page Directory
    __si->lmm.sys_info;    // System Info
    __si->lmm.sys_code;    // OS Code Segment
    __si->lmm.sys_data;    // OS Data Segment
    __si->lmm.sys_stack;   // OS Stack Segment
    __si->lmm.mach1;       // Machine specific entries
    __si->lmm.mach2;
    __si->lmm.mach3;	

    __si->iomm.locator;
    __si->iomm.phy_addr;
    __si->iomm.log_addr;
    __si->iomm.size;*/

    
}

// Must change these vectors into a single funcion, replacing 
// the table jumps with calls. Current scheme takes 1K more than ideal.

void __vector_1(void) __attribute__ ((signal));
void __vector_2(void) __attribute__ ((signal));
void __vector_3(void) __attribute__ ((signal));
void __vector_4(void) __attribute__ ((signal));
void __vector_5(void) __attribute__ ((signal));
void __vector_6(void) __attribute__ ((signal));
void __vector_7(void) __attribute__ ((signal));
void __vector_8(void) __attribute__ ((signal));
void __vector_9(void) __attribute__ ((signal));
void __vector_10(void) __attribute__ ((signal));
void __vector_11(void) __attribute__ ((signal));
void __vector_12(void) __attribute__ ((signal));
void __vector_13(void) __attribute__ ((signal));
void __vector_14(void) __attribute__ ((signal));
void __vector_15(void) __attribute__ ((signal));
void __vector_16(void) __attribute__ ((signal));
void __vector_17(void) __attribute__ ((signal));
void __vector_18(void) __attribute__ ((signal));
void __vector_19(void) __attribute__ ((signal));
void __vector_20(void) __attribute__ ((signal));
void __vector_21(void) __attribute__ ((signal));
void __vector_22(void) __attribute__ ((signal));
void __vector_23(void) __attribute__ ((signal));
void __vector_24(void) __attribute__ ((signal));
void __vector_25(void) __attribute__ ((signal));
void __vector_26(void) __attribute__ ((signal));
void __vector_27(void) __attribute__ ((signal));
void __vector_28(void) __attribute__ ((signal));
void __vector_29(void) __attribute__ ((signal));
void __vector_30(void) __attribute__ ((signal));
void __vector_31(void) __attribute__ ((signal));
void __vector_32(void) __attribute__ ((signal));
void __vector_33(void) __attribute__ ((signal));
void __vector_34(void) __attribute__ ((signal));
void __vector_35(void) __attribute__ ((signal));
void __vector_36(void) __attribute__ ((signal));

void __vector_1(void) {
	(AVRMCU::int_handler(1))();
}

void __vector_2(void) {
	(AVRMCU::int_handler(2))();
}

void __vector_3(void) {
	(AVRMCU::int_handler(3))();
}

void __vector_4(void) {
	(AVRMCU::int_handler(4))();
}

void __vector_5(void) {
	(AVRMCU::int_handler(5))();
}

void __vector_6(void) {
	(AVRMCU::int_handler(6))();
}

void __vector_7(void) {
	(AVRMCU::int_handler(7))();
}

void __vector_8(void) {
	(AVRMCU::int_handler(8))();
}

void __vector_9(void) {
	(AVRMCU::int_handler(9))();
}

void __vector_10(void) {
	(AVRMCU::int_handler(10))();
}

void __vector_11(void) {
	(AVRMCU::int_handler(11))();
}

void __vector_12(void) {
	(AVRMCU::int_handler(12))();
}

void __vector_13(void) {
	(AVRMCU::int_handler(13))();
}

void __vector_14(void) {
	(AVRMCU::int_handler(14))();
}

void __vector_15(void) {
	(AVRMCU::int_handler(15))();
}

void __vector_16(void) {
	(AVRMCU::int_handler(16))();
}

void __vector_17(void) {
	(AVRMCU::int_handler(17))();
}

void __vector_18(void) {
	(AVRMCU::int_handler(18))();
}

void __vector_19(void) {
	(AVRMCU::int_handler(19))();
}

void __vector_20(void) {
	(AVRMCU::int_handler(20))();
}

void __vector_21(void) {
	(AVRMCU::int_handler(21))();
}

void __vector_22(void) {
	(AVRMCU::int_handler(22))();
}

void __vector_23(void) {
	(AVRMCU::int_handler(23))();
}

void __vector_24(void) {
	(AVRMCU::int_handler(24))();
}

void __vector_25(void) {
	(AVRMCU::int_handler(25))();
}

void __vector_26(void) {
	(AVRMCU::int_handler(26))();
}

void __vector_27(void) {
	(AVRMCU::int_handler(27))();
}

void __vector_28(void) {
	(AVRMCU::int_handler(28))();
}

void __vector_29(void) {
	(AVRMCU::int_handler(29))();
}

void __vector_30(void) {
	(AVRMCU::int_handler(30))();
}

void __vector_31(void) {
	(AVRMCU::int_handler(31))();
}

void __vector_32(void) {
	(AVRMCU::int_handler(32))();
}

void __vector_33(void) {
	(AVRMCU::int_handler(33))();
}

void __vector_34(void) {
	(AVRMCU::int_handler(34))();
}

void __vector_35(void) {
	(AVRMCU::int_handler(35))();
}

void __vector_36(void) {
	(AVRMCU::int_handler(36))();
}

}


