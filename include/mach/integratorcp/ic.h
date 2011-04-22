#ifndef __integrator_ic_h
#define __integrator_ic_h

#include <ic.h>
#include <utility/debug.h>
#include <cpu.h>
#include <system/memory_map.h>

__BEGIN_SYS

class IntegratorCP_IC: public IC_Common
{
protected:
    
    static const CPU::Reg32 PIC_IRQ_STATUS    = 0x14000000; // RO
    static const CPU::Reg32 PIC_IRQ_RAWSTAT   = 0x14000004; // RO
    static const CPU::Reg32 PIC_IRQ_ENABLESET = 0x14000008; // RW
    static const CPU::Reg32 PIC_IRQ_ENABLECLR = 0x1400000C; // WO
    static const CPU::Reg32 PIC_INT_SOFTSET   = 0x14000010; // RW
    static const CPU::Reg32 PIC_INT_SOFTCLR   = 0x14000014; // WO
    static const CPU::Reg32 PIC_FIQ_STATUS    = 0x14000020;
    static const CPU::Reg32 PIC_FIQ_ENABLESET = 0x14000028;
    
    // FIQ/IRQ routing 
    // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0184b/CHDGGDHE.html
    
    static const CPU::Reg32 CM_FIQ_ENSET      = 0x10000068;
    static const CPU::Reg32 CM_IRQ_ENSET      = 0x10000048;
    

    
    typedef void (*Interrupt_Handler)();
    static Interrupt_Handler vector[29];
    
    typedef unsigned char Interrupt_Id;


    IntegratorCP_IC() {}

public:
	
	static const Interrupt_Id TS_PENINT = 28; // touch screen
    static const Interrupt_Id ETH_INT   = 27;
    static const Interrupt_Id CPPLDINT  = 26; // secondary IC
    static const Interrupt_Id AACIINT   = 25;
    static const Interrupt_Id MMCIINT1  = 24;
    static const Interrupt_Id MMCIINT2  = 23;
    static const Interrupt_Id CLCDCINT  = 22; // display
    static const Interrupt_Id LM_LLINT1 = 10; // logic module
    static const Interrupt_Id LM_LLINT0 = 9;
    static const Interrupt_Id RTCINT    = 8;
    static const Interrupt_Id TIMERINT2 = 7;
    static const Interrupt_Id TIMERINT1 = 6;
    static const Interrupt_Id TIMERINT0 = 5;
    static const Interrupt_Id MOUSEINT  = 4;
    static const Interrupt_Id KBDINT    = 3;
    static const Interrupt_Id UARTINT1  = 2;
    static const Interrupt_Id UARTINT0  = 1;
    static const Interrupt_Id SOFTINT   = 0;

    static void enable() {
        db<IntegratorCP_IC>(TRC) << "IC::enable()\n";
        CPU::out32(PIC_IRQ_ENABLESET, 0xFFFFFFFF);
    }

    static void enable(Interrupt_Id id) {
        // test, disable FIQ
        CPU::out32(PIC_FIQ_ENABLESET, 0);
        CPU::out32(CM_FIQ_ENSET, 0);
        CPU::out32(CM_IRQ_ENSET, 0);
        
        db<IntegratorCP_IC>(TRC) << "IC::enable(id=" << (int)id << ")\n";
        CPU::out32(PIC_IRQ_ENABLESET, CPU::in32(PIC_IRQ_ENABLESET) | (1UL << id) );
    }

    static void disable() {
        db<IntegratorCP_IC>(TRC) << "IC::disable()\n";
        CPU::out32(PIC_IRQ_ENABLESET, 0);
    }

    static void disable(Interrupt_Id id) { 
        db<IntegratorCP_IC>(TRC) << "IC::disable(id=" << (int)id << ")\n";
        CPU::out32(PIC_IRQ_ENABLESET, CPU::in32(PIC_IRQ_ENABLESET) & ~(1UL << id) );    
    }

    static void int_vector(Interrupt_Id id,Interrupt_Handler func) {
        db<IntegratorCP_IC>(TRC) << "IC::int_vector(id=" << (int)id << ",hdr="<< (void*)func <<")\n";
        vector[id] = func;
    }

    static void handle_int();
    static void default_handler();
};



__END_SYS

#endif
