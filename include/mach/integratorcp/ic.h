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
        db<IC>(TRC) << "IC::enable()\n";
        CPU::out32(PIC_IRQ_ENABLESET, 0xFFFFFFFF);
    }

    static void enable(Interrupt_Id id) {
        db<IC>(TRC) << "IC::enable(id=" << (int)id << ")\n";
        *(volatile CPU::Reg32*)PIC_IRQ_ENABLESET |= (1L << id);
    }

    static void disable() {
        db<IC>(TRC) << "IC::disable()\n";
        *(volatile CPU::Reg32*)PIC_IRQ_ENABLESET = 0;
    }

    static void disable(Interrupt_Id id) { 
        db<IC>(TRC) << "IC::disable(id=" << (int)id << ")\n";
        *(volatile CPU::Reg32*)PIC_IRQ_ENABLESET &= ~(1L << id);    
    }

    static void int_vector(Interrupt_Id id,Interrupt_Handler func) {
        db<IC>(TRC) << "IC::int_vector(id=" << (int)id << ",hdr="<< (void*)func <<")\n";
        vector[id] = func;
    }

    static void handle_int() {
        db<IC>(TRC) << "IC::handle_int()\n";
        
        CPU::Reg32 status = CPU::in32(PIC_IRQ_STATUS);
        CPU::Reg32 bit = 1;
        for(int i=0;i<29;++i) {
            if ((status & bit) && (vector[i] != 0))
                    (*vector[i])();
            bit = bit << 1;
        }
        *(volatile CPU::Reg32*)PIC_IRQ_ENABLECLR = 0xFFFFFFFF;
    }

};



__END_SYS

#endif
