// EPOS RISC-V 32 CPU Mediator Implementation

#include <architecture/rv32/rv32_cpu.h>
#include <system.h>

__BEGIN_SYS

// Class attributes
unsigned int CPU::_cpu_clock;
unsigned int CPU::_bus_clock;

// Class methods
void CPU::Context::save() volatile
{
    ASM("       csrr     x4,  mstatus           \n"
        "       sw       x4, -120(sp)           \n"     // push st
        "       la       x4,      pc            \n"
        "       sw       x4, -116(sp)           \n"     // push pc
        "       sw       x1, -112(sp)           \n"     // push ra
        "       sw       x5, -108(sp)           \n"     // push x5-x31
        "       sw       x6, -104(sp)           \n"
        "       sw       x7, -100(sp)           \n"
        "       sw       x8,  -96(sp)           \n"
        "       sw       x9,  -92(sp)           \n"
        "       sw      x10,  -88(sp)           \n"
        "       sw      x11,  -84(sp)           \n"
        "       sw      x12,  -80(sp)           \n"
        "       sw      x13,  -76(sp)           \n"
        "       sw      x14,  -72(sp)           \n"
        "       sw      x15,  -68(sp)           \n"
        "       sw      x16,  -64(sp)           \n"
        "       sw      x17,  -60(sp)           \n"
        "       sw      x18,  -56(sp)           \n"
        "       sw      x19,  -52(sp)           \n"
        "       sw      x20,  -48(sp)           \n"
        "       sw      x21,  -44(sp)           \n"
        "       sw      x22,  -40(sp)           \n"
        "       sw      x23,  -36(sp)           \n"
        "       sw      x24,  -32(sp)           \n"
        "       sw      x25,  -28(sp)           \n"
        "       sw      x26,  -24(sp)           \n"
        "       sw      x27,  -20(sp)           \n"
        "       sw      x28,  -16(sp)           \n"
        "       sw      x29,  -12(sp)           \n"
        "       sw      x30,   -8(sp)           \n"
        "       sw      x31,   -4(sp)           \n");

    ASM("       addi     sp, sp, -120           \n"                     // complete the pushes above by adjusting the SP
        "       sw       sp, 0(%0)              \n" : : "r"(this));     // update the this pointer to match the context saved on the stack
}

// Context load does not verify if interrupts were previously enabled by the Context's constructor
// We are setting mstatus to MPP | MPIE, therefore, interrupts will be enabled only after mret
void CPU::Context::load() const volatile
{
    ASM("       mv      sp, %0                  \n"                     // load the stack pointer with the this pointer
        "       addi    sp, sp, 120             \n" : : "r"(this));     // adjust the stack pointer to match the subsequent series of pops

    ASM("       lw       x1, -112(sp)           \n"     // pop ra
        "       lw       x5, -108(sp)           \n"     // pop x5-x31
        "       lw       x6, -104(sp)           \n"
        "       lw       x7, -100(sp)           \n"
        "       lw       x8,  -96(sp)           \n"
        "       lw       x9,  -92(sp)           \n"
        "       lw      x10,  -88(sp)           \n"
        "       lw      x11,  -84(sp)           \n"
        "       lw      x12,  -80(sp)           \n"
        "       lw      x13,  -76(sp)           \n"
        "       lw      x14,  -72(sp)           \n"
        "       lw      x15,  -68(sp)           \n"
        "       lw      x16,  -64(sp)           \n"
        "       lw      x17,  -60(sp)           \n"
        "       lw      x18,  -56(sp)           \n"
        "       lw      x19,  -52(sp)           \n"
        "       lw      x20,  -48(sp)           \n"
        "       lw      x21,  -44(sp)           \n"
        "       lw      x22,  -40(sp)           \n"
        "       lw      x23,  -36(sp)           \n"
        "       lw      x24,  -32(sp)           \n"
        "       lw      x25,  -28(sp)           \n"
        "       lw      x26,  -24(sp)           \n"
        "       lw      x27,  -20(sp)           \n"
        "       lw      x28,  -16(sp)           \n"
        "       lw      x29,  -12(sp)           \n"
        "       lw      x30,   -8(sp)           \n"
        "       lw      x31,   -4(sp)           \n"
        "       lw       x4, -120(sp)           \n"     // pop st
        "       csrs    mstatus,   x4           \n"     // set mstatus for mret
        "       lw       x4, -116(sp)           \n"     // pop pc
        "       csrw     mepc,     x4           \n"     // move pc to mepc for mret
        "       mret                            \n");
}

void CPU::switch_context(Context ** o, Context * n)
{   
    // Push the context into the stack and update "o"
    ASM("       sw       x1, -116(sp)           \n"     // push the return address as pc
        "       sw       x1, -112(sp)           \n"     // push ra
        "       sw       x5, -108(sp)           \n"     // push x5-x31
        "       sw       x6, -104(sp)           \n"
        "       sw       x7, -100(sp)           \n"
        "       sw       x8,  -96(sp)           \n"
        "       sw       x9,  -92(sp)           \n"
        "       sw      x10,  -88(sp)           \n"
        "       sw      x11,  -84(sp)           \n"
        "       sw      x12,  -80(sp)           \n"
        "       sw      x13,  -76(sp)           \n"
        "       sw      x14,  -72(sp)           \n"
        "       sw      x15,  -68(sp)           \n"
        "       sw      x16,  -64(sp)           \n"
        "       sw      x17,  -60(sp)           \n"
        "       sw      x18,  -56(sp)           \n"
        "       sw      x19,  -52(sp)           \n"
        "       sw      x20,  -48(sp)           \n"
        "       sw      x21,  -44(sp)           \n"
        "       sw      x22,  -40(sp)           \n"
        "       sw      x23,  -36(sp)           \n"
        "       sw      x24,  -32(sp)           \n"
        "       sw      x25,  -28(sp)           \n"
        "       sw      x26,  -24(sp)           \n"
        "       sw      x27,  -20(sp)           \n"
        "       sw      x28,  -16(sp)           \n"
        "       sw      x29,  -12(sp)           \n"
        "       sw      x30,   -8(sp)           \n"
        "       sw      x31,   -4(sp)           \n"
        "       csrr    x31,  mstatus           \n"     // get mstatus
        "       sw      x31, -120(sp)           \n"     // push st
        "       addi     sp,      sp,   -120    \n"     // complete the pushes above by adjusting the SP
        "       sw       sp,    0(a0)           \n");   // update Context * volatile * o

    // Set the stack pointer to "n" and pop the context from the stack
    ASM("       mv       sp,      a1            \n"     // get Context * volatile n into SP
        "       addi     sp,      sp,    120    \n"     // adjust stack pointer as part of the subsequent pops
        "       lw      x31, -116(sp)           \n"     // pop pc to a temporary
        "       csrw    mepc, x31               \n"
        "       lw       x1, -112(sp)           \n"     // pop ra
        "       lw       x5, -108(sp)           \n"     // pop x5-x31
        "       lw       x6, -104(sp)           \n"
        "       lw       x7, -100(sp)           \n"
        "       lw       x8,  -96(sp)           \n"
        "       lw       x9,  -92(sp)           \n"
        "       lw      x10,  -88(sp)           \n"
        "       lw      x11,  -84(sp)           \n"
        "       lw      x12,  -80(sp)           \n"
        "       lw      x13,  -76(sp)           \n"
        "       lw      x14,  -72(sp)           \n"
        "       lw      x15,  -68(sp)           \n"
        "       lw      x16,  -64(sp)           \n"
        "       lw      x17,  -60(sp)           \n"
        "       lw      x18,  -56(sp)           \n"
        "       lw      x19,  -52(sp)           \n"
        "       lw      x20,  -48(sp)           \n"
        "       lw      x21,  -44(sp)           \n"
        "       lw      x22,  -40(sp)           \n"
        "       lw      x23,  -36(sp)           \n"
        "       lw      x24,  -32(sp)           \n"
        "       lw      x25,  -28(sp)           \n"
        "       lw      x26,  -24(sp)           \n"
        "       lw      x27,  -20(sp)           \n"
        "       lw      x28,  -16(sp)           \n"
        "       lw      x29,  -12(sp)           \n"
        "       lw      x31, -120(sp)           \n"     // pop st
        "       li      x30, 0b11 << 11         \n"     // set x30 as machine mode bits on MPP
        "       or      x31, x31, x30           \n"     // machine mode on MPP is obligatory to avoid errors on mret
        "       csrw     mstatus, x31           \n"
        "       lw      x30,   -8(sp)           \n"
        "       lw      x31,   -4(sp)           \n"
        "       mret                            \n");
}

__END_SYS

/*
previously on line 167
        "       andi    x30, x31, 1 << 3        \n"     // andi with mie
        "       beqz    x30, .interrupt_ok      \n"     // if mie == 0, interrupt is ok, else, need to adjust x31 prior to write mstatus
        "       xori    x31, x31, 1 << 3        \n"     // disabling mie (we don't want interrupts to be enabled prior to the execution of mret)
        "       ori     x31, x31, 1 << 7        \n"     // enabling MPP (interrupts were enabled, mret will enable them again)
        ".interrupt_ok:                         \n"

Why did we remove:
Case1: Thread next first dispatch (default mstatus = MPIE and MPP)
    - Do not require changing mstatus
    - mret goes to thread entry_point

Case2: Next reach this code with !MIE:
    -  Next has been a previous thread before
    -  Preemption (interrupt), Yield
    -  2.1 If preemption, MIE will be restored by MRET (MPIE set prior to IC::entry())
    -  2.2 If Yield, MIE will be set by CPU::int_enable() after unlock() at Thread::dispatch().

Conclusion, no matter the case, after the changes at mstatus deafault configuration, MIE is correctly handled, and never enters switch_context() as true.
*/
