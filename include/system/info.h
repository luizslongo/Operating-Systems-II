// EPOS-- Run-Time System Information

#ifndef __info_h
#define __info_h

#include <system/config.h>

__BEGIN_SYS

class System_Info
{
private:
    typedef unsigned int LAddr;
    typedef unsigned int PAddr;
    typedef unsigned int Size;

public:
    // The information we have at boot time (built up by MKBI)
    struct Boot_Map
    {
	PAddr mem_base;          // Memory base 
	Size mem_size;          // Memory size (in bytes)
// 	struct {
// 	    unsigned int multiprocessor:1;
// 	    unsigned int multitasking:1;
// 	    unsigned int multithreading:1;
// 	} setup_mode;            // Setup mode
// 	int cpu_type;            // Processor type 
// 	int cpu_clock;           // Processor clock frequency in Hz 
// 	int n_threads;           // Max number of threads 
// 	int n_tasks;             // Max number of tasks 
	short node_id;           // Local node id in SAN (-1 => RARP) 
	short n_nodes;           // Number of nodes in SAN (-1 => dynamic) 
	Size img_size;           // Boot image size (in bytes)
	int setup_off;           // Image offsets (-1 => not present) 
	int init_off; 
	int system_off; 
	int loader_off; 
	int app_off;
    };

    // Physical Memory Map (built up by SETUP)
    struct Physical_Memory_Map
    {
	PAddr app_lo;        // Application memory's lowest address
	PAddr app_hi;        // Application memory's highest address
 	PAddr img;           // Extra applications in the Boot Image
 	Size img_size;       // Extra applications size
	PAddr int_vec;       // Interrupt Vector
	PAddr sys_pt;        // System Page Table
	PAddr sys_pd;        // System Page Directory
	PAddr sys_info;      // System Info
	PAddr phy_mem_pts;   // Page tables to map the whole physical memory
	PAddr io_mem_pts;    // Page tables to map the I/O address space 
	PAddr sys_code;      // OS Code Segment
	PAddr sys_data;      // OS Data Segment
	PAddr sys_stack;     // OS Stack Segment
	PAddr free;          // Free memory base
	Size free_size;      // Free memory size (in frames)
	PAddr io_mem;        // Memory mapped I/O base address
	Size io_mem_size;    // Memory mapped I/O size (in frames)
	PAddr mach1;         // Machine specific entries
	PAddr mach2;
	PAddr mach3;
    };

    // Logical Memory Map (built up by MKBI and SETUP)
    struct Logical_Memory_Map
    {
// 	LAddr base;            // Lowest valid logical address
// 	LAddr top;             // Highest valid logical address
// 	LAddr app_lo;          // Application memory lowest address
	LAddr app_entry;       // First application's entry point
// 	LAddr app_code;        // First application's code segment address
// 	LAddr app_data;        // First application's data segment address
	LAddr app_hi;          // Application memory highest address
// 	LAddr phy_mem;         // Whole physical memory (contiguous)
// 	LAddr io_mem;          // IO address space 
// 	LAddr int_vec;         // Interrupt Vector
// 	LAddr sys_pt;          // System Page Table
// 	LAddr sys_pd;          // System Page Directory
// 	LAddr sys_info;        // System Info
// 	LAddr sys_code;        // OS Code Segment
// 	LAddr sys_data;        // OS Data Segment
// 	LAddr sys_stack;       // OS Stack Segment
// 	LAddr mach1;           // Machine specific entries
// 	LAddr mach2;
// 	LAddr mach3;
    };

//     // I/O Device mapping information
//     struct IO_Memory_Map
//     {
// 	int locator;
// 	PAddr phy_addr;
// 	LAddr log_addr;
// 	unsigned int size;
//     };
    
public:
    Size mem_size;   // Memory size (in pages)
    Size mem_free;   // Free memory (in pages)
//    unsigned int iomm_size;
    Boot_Map bm;
    Physical_Memory_Map pmm;
    Logical_Memory_Map lmm;
//    IO_Memory_Map iomm[];
};

template <class Machine>
struct Memory_Map {};

__END_SYS

#include __HEADER_MACH(memory_map)

#endif
