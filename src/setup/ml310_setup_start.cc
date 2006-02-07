// ML310_SETUP Start Function
//
// Author: Hugo
// Documentation: $EPOS/doc/setup                        Date: 22 Sep 2003

#include <utility/elf.h>
#include <utility/ostream.h>
#include <utility/debug.h>
#include <arch/ppc32/cpu.h>
#include <mach/ml310/machine.h>
#include <mach/ml310/timer.h>
#include <arch/ppc32/mmu.h>
#include <mach/ml310/ic.h>
#include <utility/string.h>

__USING_SYS

// PPC32 Imports
typedef CPU::Reg8 Reg8;
typedef CPU::Reg16 Reg16;
typedef CPU::Phy_Addr Phy_Addr;
typedef CPU::Log_Addr Log_Addr;

// System_Info Imports
typedef System_Info::Physical_Memory_Map PMM;
typedef System_Info::Logical_Memory_Map LMM;
typedef System_Info::Boot_Map BM;
typedef Memory_Map<ML310> MM;

// Prototypes
extern "C" { int _start(char * setup_addr, int setup_size); };
void copy_sys_info(System_Info *, System_Info *);
void setup_lmm(LMM *, Log_Addr, Log_Addr);
void call_next(register Log_Addr);
void panic();
void setup_int();

__BEGIN_SYS

OStream kout, kerr;

__END_SYS

//========================================================================
// _start
//
// Desc: 
//
// Parm: 
// 
//------------------------------------------------------------------------
int _start(char * setup_addr, int setup_size)
{
    *((volatile unsigned int *)(Traits<Machine>::LEDS_BASEADDR)) = 0xFFFFFFFC;

    char * bi = (char *) (Traits<ML310>::BOOT_IMAGE_ADDR);

    db<Setup>(TRC) << "ML310_Setup(stp=" << (void *)setup_addr
                   << ",stp_sz=" << setup_size
                   << ",bi=" << (void *)bi << ")\n";

    kout << "Entering ML310 Setup\n";

    PPC32 cpu;

    // Get the System_Info, the first thing after boot
    System_Info * si = ( (System_Info *) bi);

    // Move EPOS the most close from Setup
    int epos_elfimg_size = (si->bm.img_size - si->bm.loader_off);
    char * epos_elfimg_addr = (char *) ( ((int) ((int(*)(char*,int)) &_start)) - epos_elfimg_size);
    memcpy((void *)(epos_elfimg_addr),  &bi[si->bm.loader_off], epos_elfimg_size);
    si->bm.loader_off = -1;

    //Check EPOS integrity and get the its size (code+data)
    ELF * elf = (ELF *) epos_elfimg_addr;
    if(!elf->valid()) {
        kerr << "EPOS image is corrupted!\n";
        panic();
    }

    // Get EPOS entry point and code+data segment size (aligned)
    int epos_size = 0;
    char * epos_entry = (char *)elf->entry();
    char * epos_addr = (char *)elf->segment_address(0);
    for (int i = 0;i < elf->segments();i++)
        epos_size += elf->segment_size(i);

    if ( ( ((int) epos_addr) + epos_size)   > 
         ( ((int) epos_elfimg_addr) + epos_elfimg_size) ) {
        kerr << "EPOS code+data segment is too large!\n";
        panic();
    } 

    // Say hi! :-)
    kout << "Setting up this node as follow:\n";
    kout << "Processor: PowerPC 405\n";
    kout << "Memory: " << si->bm.mem_size/1024 << " Kbytes\n";
    kout << "Node Id: ";
    if(si->bm.node_id != -1)
        kout << si->bm.node_id << " (" << si->bm.n_nodes << ")\n";
    else
        kout << "will get from the network!\n";
    kout << "EPOS: " << epos_size << " bytes\n";

    si->mem_size = si->bm.mem_size;
    si->mem_free = si->mem_size;

    // Allocate (reserve) memory for all entities we have to setup.

    // SYS_DATA
    si->mem_free -= (0x03D00000 - MM::SYS_DATA);
    si->pmm.sys_data = MM::SYS_DATA;

    // SYS_CODE
    si->mem_free -= (MM::SYS_DATA - MM::SYS_CODE);
    si->pmm.sys_code = MM::SYS_CODE;

    // INT_VEC
    si->mem_free -= (MM::SYS_INFO - MM::INT_VEC);
    si->pmm.int_vec = MM::INT_VEC;

    // SYS_STACK
    si->mem_free -= MM::SYS_STACK - 0x03D00000;
    si->pmm.sys_stack = MM::SYS_STACK;

    // SYS INFO
    si->mem_free -= (0x04000000 - MM::SYS_INFO);
    si->pmm.sys_info = MM::SYS_INFO;

    // APP_CODE
    si->mem_free -= (MM::APP_DATA - MM::APP_LO);
    // APP_DATA
    si->mem_free -= (0x00400000 - MM::APP_DATA);

    // All memory between this is free to applications
    si->pmm.app_lo = MM::APP_LO;
    si->pmm.app_hi = MM::APP_HI;

    // MACHs
    si->pmm.mach1 = MM::MACH1;
    si->pmm.mach2 = MM::MACH2;
    si->pmm.mach3 = MM::MACH3;

    //Remove System HEAP ?? Verify consistency ...
    //si->mem_free -= (0x03CEFFF8 - 0x03800000);

    // Setup Free Pointer Address (VALORACAO ALGEBRICA INTUITIVA :-)
    si->pmm.free = 0x00400000;
    si->pmm.free_size = si->mem_free;

    // Test if we didn't overlap something
    if ((si->pmm.app_lo + si->mem_free) > MM::SYS_STACK)  {
            kerr << "APPLO >> " << si->mem_free << ">" << MM::SYS_STACK << "\n";
            kerr << "Too large EPOS!\n";
            panic();
    }

    // Setup Logical Memory Map
    setup_lmm(&si->lmm, (Log_Addr)epos_entry, si->pmm.app_hi);

    // Setup the interrupt controller
    //IC ic;
    setup_int();

    // Copy SysInfo for your final destination
    copy_sys_info(si,((System_Info *) MM::SYS_INFO));

    //Load EPOS
    elf = (ELF *) epos_elfimg_addr;
    for (int i = 0;i < elf->segments();i++) {
            if(elf->load_segment(i) < 0) {
            kerr << "Load EPOS failed, image could be corrupted!\n";
            panic();
        }
    }

    // Set PCI Memory Region to work on little endian Mode (Verify a more flexible way
    // to do this !
    //Set SLER
    CPU::_mtspr(CPU::SLER, 0x0F000000);
    //Set SGR
    CPU::_mtspr(CPU::SGR, 0x0F000000);

    // Enable the Interrupt Controller to propagate interrups but keep
    // them disabled on the CPU
    // ic.enable();
    cpu.int_disable();

    *((volatile unsigned int *)(Traits<Machine>::LEDS_BASEADDR)) = 0xFFFFFFF0;

    // Call EPOS
    call_next((Log_Addr)epos_entry);

    // SETUP is now part of the free memory and this point should never be 
    // reached, but, just for ... :-) 
    panic();

    // Just to avoid the warning 
    return -1;
}

