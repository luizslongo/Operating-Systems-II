#include <system/config.h>

__USING_SYS

extern "C" unsigned __bss_end;

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
    __si->lmm.io_mem;      // IO address space 
    __si->lmm.int_vec;     // Interrupt Vector
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
}
