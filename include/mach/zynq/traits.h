#ifndef __panda_traits_h
#define __panda_traits_h

__BEGIN_SYS

template <> struct Traits<Zynq>: public Traits<void>
{
	static const unsigned int MAX_CPUS = 2;
	static const unsigned int CPUS = MAX_CPUS;
	static const unsigned int CLOCK = 667000000;
	static const unsigned int APPLICATION_STACK_SIZE = 1024*4096;
	static const unsigned int APPLICATION_HEAP_SIZE  = 64*1024*1024;
	static const unsigned int SYSTEM_HEAP_SIZE = APPLICATION_HEAP_SIZE / 2;

	static const unsigned int APP_CODE = 0; //To place the vector table at 0x0.
	static const unsigned int APP_DATA = 0x200000; //2mb

	static const unsigned int SYS = 0x100000;
	static const unsigned int SYS_HEAP = 0x300000;

	static const unsigned int MEM_BASE = 0;
	static const unsigned int MEM_TOP = 0x1fffffff;

	static const unsigned int BOOT_LENGTH_MIN = 128;
	static const unsigned int BOOT_LENGTH_MAX = 512;
	

	//static const unsigned int PRIORITY_HEAP_BASE_ADDR = 0;
	//static const unsigned int PRIORITY_HEAP_SIZE = 4;
	//static const unsigned int PRIORITY_HEAP_TOP_ADDR = PRIORITY_HEAP_BASE_ADDR + PRIORITY_HEAP_SIZE - 1;
};

template <> struct Traits<Zynq_IC>: public Traits<void>
{
    static const bool enabled = false;
};

template <> struct Traits<Zynq_Timer>: public Traits<void>
{
    static const bool enabled = false;
};

/*template <> struct Traits<PandaBoard_NIC>: public Traits<void>
{
    typedef LIST<PandaBoard_NIC> NICS;
};*/

__END_SYS

#endif

