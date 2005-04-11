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

    //__si->bm.mem_base;              
    //__si->bm.mem_size;              
    //__si->bm.cpu_type;              
    //__si->bm.cpu_clock;             
    //__si->bm.n_threads;             
    //__si->bm.n_tasks;               
    //__si->bm.host_id;               
    //__si->bm.n_nodes;               
    //__si->bm.img_size;              
    __si->bm.setup_off	       = -1;         
    __si->bm.system_off        = -1;         
    //__si->bm.loader_off;     
    //__si->bm.app_off;        

    //__si->pmm.app_lo;        
    //__si->pmm.app_hi;        
    //__si->pmm.int_vec;       
    //__si->pmm.sys_pt;        
    //__si->pmm.sys_pd;        
    __si->pmm.sys_info	       = (unsigned)&__si;
    //__si->pmm.phy_mem_pts;   
    //__si->pmm.io_mem_pts;    
    //__si->pmm.sys_code;       
    //__si->pmm.sys_data;       
    //__si->pmm.sys_stack;     
	
    __si->pmm.free 		= (unsigned)&__bss_end;    
    __si->pmm.free_size 	= 128; 
    //__si->pmm.mach1;     
    //__si->pmm.mach2;
    //__si->pmm.mach3;


    //__si->lmm.base = 0x60;      
    //__si->lmm.top = 0x45f;      
    //__si->lmm.app_lo;      
    //__si->lmm.app_entry;   
    //__si->lmm.app_code;    
    //__si->lmm.app_data;    
    //__si->lmm.app_hi;      
    //__si->lmm.phy_mem;     
    //__si->lmm.io_mem;      
    __si->lmm.int_vec 		= (unsigned)__iv;  
    //__si->lmm.sys_pt;      
    //__si->lmm.sys_pd;      
    //__si->lmm.sys_info;    
    //__si->lmm.sys_code;    
    //__si->lmm.sys_data;    
    //__si->lmm.sys_stack;   
    //__si->lmm.mach1;       
    //__si->lmm.mach2;
    //__si->lmm.mach3;	
    
    //__si->iomm.locator;
    //__si->iomm.phy_addr;
    //__si->iomm.log_addr;
    //__si->iomm.size;*/

      return;
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

  // Timer no Atmega16
  // Verificar tratador.
  void __vector_19(void) {
    //	(AVRMCU::int_handler(19))();
  }

  void __vector_20(void) {
    (AVRMCU::int_handler(20))();
  }

  void __vector_21(void) {
    (AVRMCU::int_handler(21))();
  }


}


