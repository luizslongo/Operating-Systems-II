// ML310_SETUP:
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

extern "C" { int _start(char * setup_addr, int setup_size); };

//Interrupt Handlers Prototype
extern "C" { 
void __critical_int(); 
void __noncritical_int();
void __machine_check();
void __debug_int();
void __watchdog_int();
void __insttblmiss_int();
void __inststor_int();
void __program_int();
void __syscall_int();
void __datatblmiss_int();
void __datastor_int();
void __alignment_int();
void __timer_wrapper1();
void __timer_wrapper2();
void __fit_int();
void __pit_int();
};

void copy_sys_info(System_Info *, System_Info *);
void setup_lmm(LMM *, Log_Addr, Log_Addr);
void call_next(register Log_Addr);
void panic();
void setup_int();

__BEGIN_SYS

extern OStream kout, kerr;

__END_SYS

//========================================================================
// copy_sys_info
//
// Desc: Copy the system information block, which includes boot info
//       and the physical memory map, into its definitive place.
//
// Parm: from -> current location
//       to   -> destination
//------------------------------------------------------------------------
void copy_sys_info(System_Info * from, System_Info * to)
{
    unsigned int size = sizeof(System_Info);

    db<Setup>(TRC) << "copy_sys_info(from=" << (void *)from
                   << ",to=" << (void *)to
                   << ",size=" << size << ")\n";

    if(size > 512)
        db<Setup>(WRN) << "System_Info is too large (" << size
                       << ")!\n";

    memcpy((void *)to, (void *)from, size);
}

//========================================================================
// setup_lmm
//
// Desc: Setup the Logical_Memory_Map in System_Info. 
//
// Parm: lmm        -> logical memory map
//         app_hi        -> highest logical RAM address available to applications
//------------------------------------------------------------------------
void setup_lmm(LMM * lmm, Log_Addr app_entry, Log_Addr app_hi)
{
    lmm->app_entry = app_entry;
    lmm->app_hi = app_hi;
    db<Setup>(INF) << "lmm={"
                   << "ape="  << (void *)lmm->app_entry
                   << ",aph="  << (void *)lmm->app_hi
                   << "})\n";
}

//========================================================================
// call_epos
//
// Desc: Setup a stack for EPOS with a pointer to the boot image
//         and call it.
//
// Parm: exception stack and error code pushed by the CPU
//------------------------------------------------------------------------
void call_next(register Log_Addr entry)
{
    db<Setup>(TRC) << "call_next(e=" << (void *)entry << ")\n";
    db<Setup>(INF) << "ML310_Setup ends here!\n\n";

    ASMV("mr    1,%0\n" //setting sys_stack
         "mtctr %1  \n" //setting jump address
         "bctrl     \n" : : "r"(MM::SYS_STACK),
                            "r"((unsigned)entry));
}

//========================================================================
// panic
//
// Desc: This function is called if something goes wrong during setup,
//         including uncaught interrupts.
//------------------------------------------------------------------------
void panic()
{
    kerr << "PANIC!\n";
    for(;;);
}

//========================================================================
// SETUP of BASIC interrupt handlers
//
// Desc: This functions loads the apropriated handlers address and call it.
// Since PowerPC has two class of interrupts, the returning  command (differently
// for each class is insued here, it means that the Machine::handler() could
// be the same for critical or non-critical interrupts.
//------------------------------------------------------------------------

void __machine_check() {

   //Stack Frame Create: We allocate on stack all CPU Context, less PC and MSR since
   //they are already handle in Exception hardware (They must be handled here if is
   //enabled cascate exceptions - Maybe a conf.Feature ? -Think better latter...)
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );

   // Load the Address of Interruption from the Vector Table
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_MACHINE * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_MACHINE)); //Set Handler Parameter

   // Call the Interrupt Handler, Handle unexpected interrupts, restore the 
   // context of CPU after exception and return from interrupt.
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfci                     \n" //Return from interrupt
        );
}
void __debug_int() {
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)            \n"
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_DEBUG * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_DEBUG)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfci                     \n" //Return from interrupt
        );
}
void __watchdog_int() {
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_WATCHDOG * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_WATCHDOG)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfci                     \n" //Return from interrupt
        );
}
void __insttblmiss_int() {
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_INSTTBLMISS * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_INSTTBLMISS)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfi                      \n" //Return from interrupt
        );
}
void __inststor_int() {
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_INSTSTORE * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_INSTSTORE)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfi                      \n" //Return from interrupt
        );
}
void __program_int() {
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_PROGRAM * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_PROGRAM)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfi                      \n" //Return from interrupt
        );
}
void __syscall_int() {
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_SYSCALL * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_SYSCALL)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfi                      \n" //Return from interrupt
        );
}
void __datatblmiss_int() {
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_DATATBLMISS * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_DATATBLMISS)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfi                      \n" //Return from interrupt
        );

}
void __datastor_int() {
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_DATASTORE * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_DATASTORE)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfi                      \n" //Return from interrupt
        );

}
void __alignment_int() {
   ASMV("stwu  %r1, -152(%r1)     \n" //Create the frame
        "stmw  %r0,   24(%r1)     \n" //Save r0 - r31
        "mflr  %r3                \n" //Get Link Register
        "mfctr %r4                \n" //Get Counter Register
        "mfxer %r5                \n" //Get Fixed Point Exception Register
        "mfcr  %r6                \n" //Get Conditional Register
        "stw   %r3,  156(%r1)     \n" //Save Link Register
        "stw   %r4,   12(%r1)     \n" //Save Counter Register
        "stw   %r5,   16(%r1)     \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)     \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_ALIGNMENT * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_ALIGNMENT)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfi                      \n" //Return from interrupt
        );
}
void __timer_wrapper1() {
   ASMV("b 4352");
}
void __timer_wrapper2() {
   ASMV("b 4608");
}
void __fit_int() {
   ASMV("stwu  %r1, -152(%r1)  \n" //Create the frame
        "stmw  %r0,   24(%r1)  \n" //Save r0 - r31
        "mflr  %r3             \n" //Get Link Register
        "mfctr %r4             \n" //Get Counter Register
        "mfxer %r5             \n" //Get Fixed Point Exception Register
        "mfcr  %r6             \n" //Get Conditional Register
        "stw   %r3,  156(%r1)  \n" //Save Link Register
        "stw   %r4,   12(%r1)  \n" //Save Counter Register
        "stw   %r5,   16(%r1)  \n" //Save Fixed Point Exception Register
        "stw   %r6,   20(%r1)  \n" //Save Conditional Register
       );
   ASMV("lwz   0, 0(%0)     \n" //Load Address from Exception Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_FITIMER * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_FITIMER)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz   %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz   %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz   %r4,   12(%r1)     \n" //Load Counter Register
        "lwz   %r3,  156(%r1)     \n" //Load Link Register
        "mtcr  %r6                \n" //Put Conditional Register
        "mtxer %r5                  \n" //Put Fixed Point Register
        "mtctr %r4                  \n" //Put Counter Register
        "mtlr  %r3                  \n" //Put Link Register
        "lwz   %r0,   24(%r1)     \n" //Restore r0
        "lmw   %r2,   32(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 152          \n" //Restore Stack Pointer (r1) 
        "rfi                      \n" //Return from interrupt
        );
}
void __pit_int() {
   ASMV("stwu   %r1, -160(%r1)  \n" //Create the frame
        "stmw   %r0,   32(%r1)  \n" //Save r0 - r31
        "mflr   %r3             \n" //Get Link Register
        "mfctr  %r4             \n" //Get Counter Register
        "mfxer  %r5             \n" //Get Fixed Point Exception Register
        "mfcr   %r6             \n" //Get Conditional Register
        "mfsrr0 %r7             \n" //Get Save/Restore 0
        "mfsrr1 %r8             \n" //Get Save/Restore 1
        "stw    %r3,  164(%r1)  \n" //Save Link Register
        "stw    %r4,   12(%r1)  \n" //Save Counter Register
        "stw    %r5,   16(%r1)  \n" //Save Fixed Point Exception Register
        "stw    %r6,   20(%r1)  \n" //Save Conditional Register
        "stw    %r7,   24(%r1)  \n" //Save Save/Restore 0
        "stw    %r8,   28(%r1)  \n" //Save Save/Restore 1
        "sync                   \n" //Sync Store & Instruction execution
       );
   ASMV("mtspr %0, %1             \n" //Clear Interrupt.
        : : "i"(ML310_Timer::SPR_TSR),"r"(0x08000000) : "%0");
   ASMV("lwz   0, 0(%0)           \n" //Load Address from Interrupt Vector
        : : "r"(Traits<ML310>::INT_BASE + (IC::INT_PITIMER * 4)) : "%0");
   ASMV("li    3, %0              \n" : : "i"(IC::INT_PITIMER)); //Set Handler Parameter
   ASMV("mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n" //Halt CPU (Put in Wait State)
        //Restore Context:
        "lwz    %r8,   28(%r1)     \n" //Load Save/Restore 1
        "lwz    %r7,   24(%r1)     \n" //Load Save/Restore 0
        "lwz    %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz    %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz    %r4,   12(%r1)     \n" //Load Counter Register
        "lwz    %r3,  164(%r1)     \n" //Load Link Register
        "mtsrr1 %r8                \n" //Get Save/Restore 1
        "mtsrr0 %r7                \n" //Get Save/Restore 0
        "mtcr   %r6                \n" //Put Conditional Register
        "mtxer  %r5                   \n" //Put Fixed Point Register
        "mtctr  %r4                   \n" //Put Counter Register
        "mtlr   %r3                   \n" //Put Link Register
        //Disable Wait State if in Wait State !!!
        "mfsrr1 0                  \n" //Load MSR (on SSR1)
        "lis    3, 0xFFFB          \n" // Upper Mask of Wait State
        "ori    3, 3, 0xFFFF       \n" // Lower Mask of Wait State
        "and    0, 0, 3            \n" // Disable Wait State
        "mtsrr1 0                  \n" //Update MSR (on SSR1)
        //^--For CPU::halt() method !
        "lwz   %r0,   32(%r1)     \n" //Restore r0
        "lmw   %r2,   40(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 160          \n" //Restore Stack Pointer (r1) 
        "sync                     \n" //Sync Storage & Instruction execution
        "rfi                      \n" //Return from interrupt
        );
}
void __noncritical_int() {
   ASMV("stwu   %r1, -160(%r1)  \n" //Create the frame
        "stmw   %r0,   32(%r1)  \n" //Save r0 - r31
        "mflr   %r3             \n" //Get Link Register
        "mfctr  %r4             \n" //Get Counter Register
        "mfxer  %r5             \n" //Get Fixed Point Exception Register
        "mfcr   %r6             \n" //Get Conditional Register
        "mfsrr0 %r7             \n" //Get Save/Restore 0
        "mfsrr1 %r8             \n" //Get Save/Restore 1
        "stw    %r3,  164(%r1)  \n" //Save Link Register
        "stw    %r4,   12(%r1)  \n" //Save Counter Register
        "stw    %r5,   16(%r1)  \n" //Save Fixed Point Exception Register
        "stw    %r6,   20(%r1)  \n" //Save Conditional Register
        "stw    %r7,   24(%r1)  \n" //Save Save/Restore 0
        "stw    %r8,   28(%r1)  \n" //Save Save/Restore 1
        "sync                   \n" //Sync Store & Instruction execution
       );

        // Clear Interrupt
   ASMV("lwz   4, 0(%0)         \n" : : "r"(ML310_IC::IVR) : "%0");
   ASMV("li    5, 1             \n"
        "slw   6, 5, 4          \n"
        "stw   6, 0(%0)         \n" : : "r"(ML310_IC::IAR) : "%0");

   //volatile unsigned int ivr;
   //ASMV("lwz   %0, 0(%1)           \n" : "=r"(ivr) : "r"(ML310_IC::IVR) : "%0");
   //ivr = 1 << ivr;
   //ASMV("stw   %0, 0(%1)           \n" : : "r"(ivr), "r"(ML310_IC::IAR) : "%0");

   //ASMV("lwz   9, 0(%0)           \n" : : "r"(ML310_IC::IVR) : "%0");
   ASMV("addi  4, 4, 12           \n" // Add IRQ displacement
        "mr    3, 4               \n" // Set Handler Parameter
        "slwi  4, 4,  2           \n" // Multiply by 4
        "add   4, 4, %0           \n" : : "r"(Traits<ML310>::INT_BASE)); //Sum with base addr
   ASMV("lwz   0, 0(4)            \n" //Load Handler Addr.
        "mtctr 0                  \n" //Load Handler Address in CounterRegister
        "cmpwi 0, 0               \n" //compare If null
        "beq 12                   \n" //If == 0, branch PANIC
        "bctrl                    \n" //If != 0, branch to handler
        "b 32                     \n" //After Handler Routine, restore_context
        //Panic:
        "lis   3, 0x9000          \n" //LED UPPER ADDR
        "ori   3, 3, 0x0000       \n" //LED LOWER ADDR
        "lis   4, 0x0000          \n" //Value Upper
        "ori   4, 4, 0x0000       \n" //Value Lower
        "stw   4, 0(3)            \n" //Turn On LEDS
        "li    0, 8192            \n" //Set MSR for HALT
        "mtmsr 0                  \n"); //Halt CPU (Put in Wait State)
        //Restore Context:
   ASMV("lwz    %r8,   28(%r1)     \n" //Load Save/Restore 1
        "lwz    %r7,   24(%r1)     \n" //Load Save/Restore 0
        "lwz    %r6,   20(%r1)     \n" //Load Conditional Register
        "lwz    %r5,   16(%r1)     \n" //Load Fixed Point Exception Register
        "lwz    %r4,   12(%r1)     \n" //Load Counter Register
        "lwz    %r3,  164(%r1)     \n" //Load Link Register
        "mtsrr1 %r8                \n" //Get Save/Restore 1
        "mtsrr0 %r7                \n" //Get Save/Restore 0
        "mtcr   %r6                \n" //Put Conditional Register
        "mtxer  %r5                \n" //Put Fixed Point Register
        "mtctr  %r4                \n" //Put Counter Register
        "mtlr   %r3                \n" //Put Link Register
        "lwz   %r0,   32(%r1)     \n" //Restore r0
        "lmw   %r2,   40(%r1)     \n" //Restore r2 - r31
        "addi  1, 1, 160          \n" //Restore Stack Pointer (r1) 
        "sync                     \n" //Sync Storage & Instruction execution
        "rfi                      \n" //Return from interrupt
        );
}
//This interrupt is disable and never should occur (CPU PIN Disconnected)
void __critical_int() {
   ASMV("rfci");
}

//========================================================================
// Setup Interrupts
//
// Desc: This function copy the handlers to effective handler address
// of PowerPC.
//
//------------------------------------------------------------------------
void setup_int()
{

   //Clear interrupt structures area
   memset((void *)MM::INT_VEC, 0, 0x6400);

   //Copy handlers routine to apropriate location.
   unsigned int int_addr;
   int_addr = MM::INT_VEC + 0x0100;//Critical Interrupt
   memcpy((void *)int_addr, (void *)__critical_int, 4);
   int_addr = MM::INT_VEC + 0x0200;//Machine Check
   memcpy((void *)int_addr, (void *)__machine_check, 148);
   int_addr = MM::INT_VEC + 0x0300;//Data Storage
   memcpy((void *)int_addr, (void *)__datastor_int, 152);
   int_addr = MM::INT_VEC + 0x0400;//Instruction
   memcpy((void *)int_addr, (void *)__inststor_int, 152);
   int_addr = MM::INT_VEC + 0x0500;//External
   memcpy((void *)int_addr, (void *)__noncritical_int, 232);
   int_addr = MM::INT_VEC + 0x0600;//Alignment
   memcpy((void *)int_addr, (void *)__alignment_int, 152);
   int_addr = MM::INT_VEC + 0x0700;//Program
   memcpy((void *)int_addr, (void *)__program_int, 152);
   int_addr = MM::INT_VEC + 0x0C00;//System Call
   memcpy((void *)int_addr, (void *)__syscall_int, 152);
   int_addr = MM::INT_VEC + 0x1000;//PIT
   memcpy((void *)int_addr, (void *)__timer_wrapper1, 4);
   int_addr = MM::INT_VEC + 0x1010;//FIT
   memcpy((void *)int_addr, (void *)__timer_wrapper2, 4);
   int_addr = MM::INT_VEC + 0x1020;//WatchDog Timer
   memcpy((void *)int_addr, (void *)__watchdog_int, 152);
   int_addr = MM::INT_VEC + 0x1100;//Data TBL Miss
   memcpy((void *)int_addr, (void *)__datatblmiss_int, 152);
   int_addr = MM::INT_VEC + 0x1200;//Instruction TBL Miss
   memcpy((void *)int_addr, (void *)__insttblmiss_int, 152);
   int_addr = MM::INT_VEC + 0x2000;//Debug
   memcpy((void *)int_addr, (void *)__debug_int, 152);
   int_addr = MM::INT_VEC + 0x2100;//Debug
   memcpy((void *)int_addr, (void *)__pit_int, 220);
   int_addr = MM::INT_VEC + 0x2210;//Debug
   memcpy((void *)int_addr, (void *)__fit_int, 152);

   //Set Vector Configuration Address and EVPR
   ASMV("mtspr %1, %0" : : "r"(MM::INT_VEC), "i"(0x3D6));
   //Invalidate all instruction cache and sync.
   ASMV("iccci 0, 1; sync;");//Not portable code!???

}
