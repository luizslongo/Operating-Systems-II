// EPOS IA32 CPU Mediator Declarations

#ifndef __ia32_h
#define __ia32_h

#include <architecture/cpu.h>
#include <system/memory_map.h>

__BEGIN_SYS

class CPU: private CPU_Common
{
    friend class Init_System;
    friend class Machine;

private:
    static const bool multicore = Traits<System>::multicore;

public:
    // Bootstrap/service CPU id
    static const unsigned long BSP = 0;

    // Native Data Types
    using CPU_Common::Reg8;
    using CPU_Common::Reg16;
    using CPU_Common::Reg32;
    using CPU_Common::Reg64;
    using CPU_Common::Reg;
    using CPU_Common::Log_Addr;
    using CPU_Common::Phy_Addr;
    using CPU_Common::Interrupt_Id;

    // Flags
    typedef Reg32 Flags;
    enum {
        FLAG_CF     = 1 <<  0, // Carry
        FLAG_RES1   = 1 <<  1, // Reserved (always 1)
        FLAG_PF     = 1 <<  2, // Parity (1->even,0->odd)
        FLAG_AF     = 1 <<  4, // Auxiliary Carry
        FLAG_ZF     = 1 <<  6, // Zero
        FLAG_SF     = 1 <<  7, // Sign (1->negative,0->positive)
        FLAG_TF     = 1 <<  8, // Trap (single step)
        FLAG_IF     = 1 <<  9, // Interrupt (1->enabled,0->disabled)
        FLAG_DF     = 1 << 10, // Direction (1->down,0->up)
        FLAG_OF     = 1 << 11, // Overflow
        FLAG_IOPL   = 3 << 12, // I/O PL
        FLAG_NT     = 1 << 14, // Nested Task
        FLAG_RF     = 1 << 16, // Resume
        FLAG_VM     = 1 << 17, // Virtual 8086 mode
        FLAG_AC     = 1 << 18, // Alignment check
        FLAG_VIF    = 1 << 19, // Virtual Interrupt
        FLAG_VIP    = 1 << 20, // Virtual Interrupt Pending
        FLAG_ID     = 1 << 21, // Supports CPUID
        FLAG_DEFAULTS   = FLAG_IF,
        // Mask to clear flags (by ANDing)
        FLAG_CLEAR      = ~(FLAG_TF | FLAG_IOPL | FLAG_NT | FLAG_RF | FLAG_VM | FLAG_AC)
    };

    // Exceptions
    typedef Reg32 Exceptions;
    enum {
        EXC_BASE    = 0x00,
        EXC_DIV0    = 0x00,
        EXC_DEBUG   = 0x01,
        EXC_NMI     = 0x02,
        EXC_BP      = 0x03,
        EXC_OVFLOW  = 0x04,
        EXC_BOUND   = 0x05,
        EXC_INVOP   = 0x06,
        EXC_NODEV   = 0x07,
        EXC_DOUBLE  = 0x08,
        EXC_FPU_OR  = 0x09,
        EXC_INVTSS  = 0x0a,
        EXC_NOTPRE  = 0x0b,
        EXC_STACK   = 0x0c,
        EXC_GPF     = 0x0d,
        EXC_PF      = 0x0e,
        EXC_RESERV  = 0x0f,
        EXC_FPU     = 0x10,
        EXC_ALIGN   = 0x11,
        EXC_BUS     = 0x12,
        EXC_LAST    = 0x1f
    };

    // Flags
    enum {
        CR0_PE      = 1 <<  0, // Protected Mode Enable (0->real mode, 1->protected mode)
        CR0_MP      = 1 <<  1, // Monitor co-processor  (1->WAIT/FWAIT with TS flag)
        CR0_EM      = 1 <<  2, // Emulation             (0->x87 FPU, 1->no x87 FPU)
        CR0_TS      = 1 <<  3, // Task switched         (delayed x87 context switch)
        CR0_ET      = 1 <<  4, // Extension type        (for i386, 0->80387, 1-> 80287)
        CR0_NE      = 1 <<  5, // Numeric error         (1->internal x87 FPU error reporting, 0-> x87 style)
        CR0_WP      = 1 << 16, // Write protect         (1->CPU can't write to R/O pages in CPL=0)
        CR0_AM      = 1 << 18, // Alignment mask        (1->alignment check in CPL=3)
        CR0_NW      = 1 << 29, // Not-write through     (1->globally disable write-through caching)
        CR0_CD      = 1 << 30, // Cache disable         (1->globally disable the memory cache)
        CR0_PG      = 1 << 31, // Paging                (1->paging)
        CR0_CLEAR       = (CR0_PE | CR0_EM | CR0_WP),   // Mask to clear flags (by ANDing)
        CR0_SET         = (CR0_PE | CR0_PG)             // Mask to set flags (by ORing)
    };

    // CR4 Flags
    enum {
        CR4_PSE     = 1 << 8    // CR4 Performance Counter Enable
    };

    // Segment Flags
    enum {
        SEG_ACC         = 0x01,
        SEG_RW          = 0x02,
        SEG_CONF        = 0x04,
        SEG_CODE        = 0x08,
        SEG_NOSYS       = 0x10,
        SEG_DPL1        = 0x20,
        SEG_DPL2        = 0x40,
        SEG_PRE         = 0x80,
        SEG_TSS         = 0x09,
        SEG_INT         = 0x0e,
        SEG_TRAP        = 0x0f,
        SEG_32          = 0x40,
        SEG_4K          = 0x80,
        SEG_FLT_CODE    = (SEG_PRE  | SEG_NOSYS | SEG_CODE | SEG_RW   | SEG_ACC  ),
        SEG_FLT_DATA    = (SEG_PRE  | SEG_NOSYS | SEG_RW   | SEG_ACC  ),
        SEG_SYS_CODE    = (SEG_PRE  | SEG_NOSYS | SEG_CODE | SEG_RW   | SEG_ACC  ),
        SEG_SYS_DATA    = (SEG_PRE  | SEG_NOSYS | SEG_RW   | SEG_ACC  ),
        SEG_APP_CODE    = (SEG_PRE  | SEG_NOSYS | SEG_DPL2 | SEG_DPL1 | SEG_CODE | SEG_RW   | SEG_ACC), // P, DPL=3, S, C, W, A
        SEG_APP_DATA    = (SEG_PRE  | SEG_NOSYS | SEG_DPL2 | SEG_DPL1            | SEG_RW   | SEG_ACC), // P, DPL=3, S,    W, A
        SEG_IDT_ENTRY   = (SEG_PRE  | SEG_INT   | SEG_DPL2 | SEG_DPL1 ),
        SEG_TSS0        = (SEG_PRE  | SEG_TSS   | SEG_DPL2 | SEG_DPL1 )
    };

    // DPL/RPL for application (user) and system (supervisor) modes
    enum {
        PL_APP = 3, // GDT, RPL=3
        PL_SYS = 0  // GDT, RPL=0
    };

    // GDT Layout
    enum GDT_Layout { // GCC BUG (anonymous enum in templates)
        GDT_NULL      = 0,
        GDT_FLT_CODE  = 1,
        GDT_FLT_DATA  = 2,
        GDT_SYS_CODE  = GDT_FLT_CODE,
        GDT_SYS_DATA  = GDT_FLT_DATA,
        GDT_APP_CODE  = 3,
        GDT_APP_DATA  = 4,
        GDT_TSS0      = 5
    };

    // GDT Selectors
    enum {
        SEL_FLT_CODE  = (GDT_FLT_CODE << 3)  | PL_SYS,
        SEL_FLT_DATA  = (GDT_FLT_DATA << 3)  | PL_SYS,
        SEL_SYS_CODE  = (GDT_SYS_CODE << 3)  | PL_SYS,
        SEL_SYS_DATA  = (GDT_SYS_DATA << 3)  | PL_SYS,
        SEL_APP_CODE  = (GDT_APP_CODE << 3)  | PL_APP,
        SEL_APP_DATA  = (GDT_APP_DATA << 3)  | PL_APP,
        SEL_TSS0      = (GDT_TSS0     << 3)  | PL_SYS
    };

    // Useful MSRs
    enum {
        MSR_TSC                 = 0x0010,
        CLOCK_MODULATION        = 0x019a,
        THERM_STATUS            = 0x019c,
        TEMPERATURE_TARGET      = 0x01a2
    };

    // GDT Entry
    class GDT_Entry {
    public:
        GDT_Entry() {}
        GDT_Entry(Reg32 b, Reg32 l, Reg8 f)
        : limit_15_00((Reg16)l), base_15_00((Reg16)b), base_23_16((Reg8)(b >> 16)), p_dpl_s_type(f),
          g_d_0_a_limit_19_16(((f & SEG_NOSYS) ? (SEG_4K | SEG_32) : 0) | ((Reg8)(l >> 16))), base_31_24((Reg8)(b >> 24)) {}

        friend OStream & operator<<(OStream & os, const GDT_Entry & g) {
            os << "{bas=" << (void *)((g.base_31_24 << 24) | (g.base_23_16 << 16) | g.base_15_00)
               << ",lim=" << (void *)(((g.g_d_0_a_limit_19_16 & 0xf) << 16) | g.limit_15_00)
               << ",p=" << (g.p_dpl_s_type >> 7)
               << ",dpl=" << ((g.p_dpl_s_type >> 5) & 0x3)
               << ",s=" << ((g.p_dpl_s_type >> 4) & 0x1)
               << ",typ=" << (g.p_dpl_s_type & 0xf)
               << ",g=" << (g.g_d_0_a_limit_19_16 >> 7)
               << ",d=" << ((g.g_d_0_a_limit_19_16 >> 6) & 0x1)
               << ",a=" << ((g.g_d_0_a_limit_19_16 >> 4) & 0x1) << "}";
            return os;
        }

    private:
        Reg16 limit_15_00;
        Reg16 base_15_00;
        Reg8  base_23_16;
        Reg8  p_dpl_s_type;
        Reg8  g_d_0_a_limit_19_16;
        Reg8  base_31_24;
    };

    // IDT Entry
    class IDT_Entry {
    public:
        IDT_Entry() {}
        IDT_Entry(Reg16 s, Reg32 o, Reg16 f)
        : offset_15_00((Reg16)o), selector(s), zero(0), p_dpl_0_d_1_1_0(f), offset_31_16((Reg16)(o >> 16)) {}

        Reg32 offset() const { return (offset_31_16 << 16) | offset_15_00; }

        friend OStream & operator<<(OStream & os, const IDT_Entry & i) {
            os << "{sel=" << i.selector
               << ",off=" << (void *)i.offset()
               << ",p=" << (i.p_dpl_0_d_1_1_0 >> 7)
               << ",dpl=" << ((i.p_dpl_0_d_1_1_0 >> 5) & 0x3)
               << ",d=" << ((i.p_dpl_0_d_1_1_0 >> 4) & 0x1) << "}";
            return os;
        }

    private:
        Reg16 offset_15_00;
        Reg16 selector;
        Reg8  zero;
        Reg8  p_dpl_0_d_1_1_0;
        Reg16 offset_31_16;
    };
    static const unsigned int IDT_ENTRIES = 256;

    // TSS no longer used, since software context switch is faster
    // it's left here for reference
    struct TSS {
        Reg16 back_link;
        Reg16 zero1;
        Reg32 esp0;
        Reg16 ss0;
        Reg16 zero2;
        Reg32 esp1;
        Reg16 ss1;
        Reg16 zero3;
        Reg32 esp2;
        Reg16 ss2;
        Reg16 zero4;
        Reg32 pdbr;
        Reg32 eip;
        Reg32 eflags;
        Reg32 eax;
        Reg32 ecx;
        Reg32 edx;
        Reg32 ebx;
        Reg32 esp3;
        Reg32 ebp;
        Reg32 esi;
        Reg32 edi;
        Reg16 es;
        Reg16 zero5;
        Reg16 cs;
        Reg16 zero6;
        Reg16 ss3;
        Reg16 zero7;
        Reg16 ds;
        Reg16 zero8;
        Reg16 fs;
        Reg16 zero9;
        Reg16 gs;
        Reg16 zero10;
        Reg16 ldt;
        Reg16 zero11;
        Reg16 zero12;
        Reg16 io_bmp;
    };

    // CPU Context
    class Context
    {
        friend class CPU;       // for Context::push() and Context::pop()
        friend class IC;        // for Context::push() and Context::pop()

    public:
        static const unsigned long PAGE_SIZE = 4096; // this does not depend on the MMU configuration; it's a fixed valued
        static const unsigned long CROSS_LEVEL_CONTEX_SIZE = 13 * sizeof(long); // 8 registers + SS + USP + FLAGS + CS + IP

    public:
        Context() {}
        Context(Log_Addr usp, Log_Addr entry): _eip(entry), _cs(SEL_SYS_CODE), _eflags(FLAG_DEFAULTS) {
            if(Traits<Build>::hysterically_debugged || Traits<Thread>::trace_idle) {
                _edi = 1; _esi = 2; _ebp = 3; _esp = 4; _ebx = 5; _edx = 6; _ecx = 7; _eax = 8;
            }
        }

        void save() volatile __attribute__ ((naked));
        void load() const volatile __attribute__ ((naked));

        friend OStream & operator<<(OStream & os, const Context & c) {
            os << hex
               << "{flags=" << c._eflags
               << ",ax=" << c._eax
               << ",bx=" << c._ebx
               << ",cx=" << c._ecx
               << ",dx=" << c._edx
               << ",si=" << c._esi
               << ",di=" << c._edi
               << ",bp=" << reinterpret_cast<void *>(c._ebp)
               << ",sp=" << &c
               << ",ip=" << reinterpret_cast<void *>(c._eip)
               << ",cs="  << c._cs
               << ",ccs=" << cs()
               << ",cds=" << ds()
               << ",ces=" << es()
               << ",cfs=" << fs()
               << ",cgs=" << gs()
               << ",css=" << ss()
               << ",cr3=" << reinterpret_cast<void *>(cr3())
               << "}"     << dec;
            return os;
        }

    private:
        static void pop(bool interrupt = false);
        static void push(bool interrupt = false);

    private:
        Reg32 _edi;
        Reg32 _esi;
        Reg32 _ebp;
        Reg32 _esp; // redundant (=this); see cpu.cc for details
        Reg32 _ebx;
        Reg32 _edx;
        Reg32 _ecx;
        Reg32 _eax;
        Reg32 _eip;
        Reg32 _cs;
        Reg32 _eflags;
    };

    // I/O ports
    typedef Reg16 IO_Port;
    typedef Reg16 IO_Irq;

    // Interrupt Service Routines
    typedef void (ISR)();

    // Fault Service Routines (exception handlers)
    typedef void (FSR)(Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags);

public:
    CPU() {}

    static Log_Addr pc() { return eip(); }

    static Log_Addr sp() { return esp(); }
    static void sp(Log_Addr sp) { esp(sp); }

    static Reg fr() { return eax(); }
    static void fr(Reg r) { eax(r); }

    static volatile unsigned int id();
    static unsigned int cores() { return multicore ? _cores : 1; }
    static void cores(unsigned int cores) {
        _cores = cores;
    }

    static Hertz clock() { return _cpu_current_clock; }
    static void clock(Hertz frequency) {
        Reg64 clock = frequency;
        unsigned int dc;
        if(clock <= (_cpu_clock * 1875ULL / 10000ULL)) {
            dc = 0b10011;   // minimum duty cycle of 12.5 %
            _cpu_current_clock = _cpu_clock * 1875ULL / 10000ULL;
        } else if(clock >= (_cpu_clock * 9375ULL / 10000ULL)) {
            dc = 0b01001;   // disable duty cycling and operate at full speed
            _cpu_current_clock = _cpu_clock;
        } else {
            dc = 0b10001 | ((clock * 10000ULL / _cpu_clock + 625ULL) / 625ULL); // dividing by 625 instead of 1250 eliminates the shift left
            _cpu_current_clock = _cpu_clock * ((clock * 10000ULL / _cpu_clock + 625ULL) / 625ULL) * 625ULL / 10000ULL;
            // The ((clock * 10000 / _cpu_clock + 625) / 625) returns the factor, the step is 625/10000
            // thus, max_clock * factor * step = final clock
        }
        wrmsr(CLOCK_MODULATION, dc);
    }
    static Hertz max_clock() { return _cpu_clock; }
    static Hertz min_clock() { return _cpu_clock * 1250 / 10000;}

    static Hertz bus_clock() { return _bus_clock; }

    static void int_enable() { ASM("sti"); }
    static void int_disable() { ASM("cli"); }
    static bool int_enabled() { return (flags() & FLAG_IF); }
    static bool int_disabled() { return !int_enabled(); }

    static void halt() { ASM("hlt"); }

    static void fpu_save() {} // TODO
    static void fpu_restore() {} // TODO

    static void switch_context(Context * volatile * o, Context * volatile n);

    template<typename T>
    static T tsl(volatile T & lock) {
        register T old = 1;
        ASM("lock xchg %0, %2" : "=a"(old) : "a"(old), "m"(lock) : "memory");
        return old;
    }

    template<typename T>
    static T finc(volatile T & value) {
        register T old = 1;
        ASM("lock xadd %0, %2" : "=a"(old) : "a"(old), "m"(value) : "memory");
        return old;
    }

    template<typename T>
    static T fdec(volatile T & value) {
        register T old = -1;
        ASM("lock xadd %0, %2" : "=a"(old) : "a"(old), "m"(value) : "memory");
        return old;
    }

    template<typename T>
    static T cas(volatile T & value, T compare, T replacement) {
        ASM("lock cmpxchg %2, %3\n" : "=a"(compare) : "a"(compare), "r"(replacement), "m"(value) : "memory");
        return compare;
    }

    static void smp_barrier(unsigned long cores = CPU::cores()) { CPU_Common::smp_barrier<&finc>(cores, id()); }

    // MMU operations
    static Reg  pd() { return cr3(); }
    static void pd(Reg r) { cr3(r); }

    static void flush_tlb() { ASM("movl %cr3, %eax"); ASM("movl %eax, %cr3"); }
    static void flush_tlb(Reg32 r) { ASM("invlpg %0" : : "m"(r)); }

    static Reg64 htole64(Reg64 v) { return v; }
    static Reg32 htole32(Reg32 v) { return v; }
    static Reg16 htole16(Reg16 v) { return v; }
    static Reg64 letoh64(Reg64 v) { return v; }
    static Reg32 letoh32(Reg32 v) { return v; }
    static Reg16 letoh16(Reg16 v) { return v; }

    static Reg64 htobe64(Reg64 v) { ASM("bswap %0" : "=r"(v) : "0"(v), "r"(v)); return v; }
    static Reg32 htobe32(Reg32 v) { ASM("bswap %0" : "=r"(v) : "0"(v), "r"(v)); return v; }
    static Reg16 htobe16(Reg16 v) { return swap16(v); }
    static Reg64 betoh64(Reg64 v) { return htobe64(v); }
    static Reg32 betoh32(Reg32 v) { return htobe32(v); }
    static Reg16 betoh16(Reg16 v) { return htobe16(v); }

    static Reg32 htonl(Reg32 v) { ASM("bswap %0" : "=r"(v) : "0"(v), "r"(v)); return v; }
    static Reg16 htons(Reg16 v) { return swap16(v); }
    static Reg32 ntohl(Reg32 v) { return htonl(v); }
    static Reg16 ntohs(Reg16 v) { return htons(v); }

    template<typename ... Tn>
    static Context * init_stack(Log_Addr usp, Log_Addr sp, void (* exit)(), int (* entry)(Tn ...), Tn ... an) {
        sp -= SIZEOF<Tn ... >::Result;
        init_stack_helper(sp, an ...);
        sp -= sizeof(int *);
        *static_cast<int *>(sp) = Log_Addr(exit);
        sp -= sizeof(Context);
        return new (sp) Context(0, entry);
    }

    template<typename ... Tn>
    static Log_Addr init_user_stack(Log_Addr usp, void (* exit)(), Tn ... an) {
        usp -= SIZEOF<Tn ... >::Result;
        init_stack_helper(usp, an ...);
        if(exit) { // only MAIN has exit = 0, because it returns via CRT0
            usp -= sizeof(int *);
            *static_cast<int *>(usp) = Log_Addr(exit);
        }
        return usp;
    }

public:
    // IA32 specifics
    static Flags flags() { Reg32 r; ASM("pushfl");              ASM("popl %0" : "=r"(r) :); return r; }
    static void flags(Flags r) {    ASM("pushl %0" : : "r"(r)); ASM("popfl"); }

    static Reg32 esp() { Reg32 r; ASM("movl %%esp,%0"  : "=r"(r) :); return r; }
    static void esp(Reg32 r) {    ASM("movl %0, %%esp" : : "X"(r)); }

    static Reg32 eax() { Reg32 r; ASM("movl %%eax,%0"  : "=r"(r) :); return r; }
    static void eax(Reg32 r) {    ASM("movl %0, %%eax" : : "X"(r)); }

    static Reg32 ecx() { Reg32 r; ASM("movl %%ecx,%0"  : "=r"(r) :); return r; }
    static void ecx(Reg32 r) {    ASM("movl %0, %%ecx" : : "X"(r)); }

    static Log_Addr eip() {
        Log_Addr value;
        ASM("       push    %%eax                           \n"
            "       call    1f                              \n"
            "1:     popl    %%eax       # ret. addr.        \n"
            "       movl    %%eax,%0                        \n"
            "       popl    %%eax                           \n" : "=o"(value) : : "%eax");
        return value;
    }

    static void cpuid(Reg32 op, Reg32 * eax, Reg32 * ebx, Reg32 * ecx, Reg32 * edx) {
        *eax = op;
        ASM("cpuid" : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx) : "0"(*eax), "2"(*ecx));
    }

    static Reg32 cr0() { Reg32 r; ASM("movl %%cr0, %0" : "=r"(r) :); return r; }
    static void cr0(Reg32 r) {    ASM("movl %0, %%cr0" : : "r"(r)); }
    static Reg32 cr2() { Reg32 r; ASM("movl %%cr2, %0" : "=r"(r) :); return r; }
    static Reg32 cr3() { Reg32 r; ASM("movl %%cr3, %0" : "=r"(r) :); return r; }
    static void cr3(Reg32 r) {    ASM("movl %0, %%cr3" : : "r"(r)); }
    static Reg32 cr4() { Reg32 r; ASM("movl %%cr4, %0" : "=r"(r) :); return r; }
    static void cr4(Reg32 r) {    ASM("movl %0, %%cr4" : : "r"(r)); }

    static void gdtr(Reg16 * limit, Reg32 * base) {
        volatile Reg8 aux[6];
        volatile Reg16 * l = reinterpret_cast<volatile Reg16 *>(&aux[0]);
        volatile Reg32 * b = reinterpret_cast<volatile Reg32 *>(&aux[2]);

        ASM("sgdt %0" : "=m"(aux[0]) :);
        *limit = *l;
        *base = *b;
    }
    static void gdtr(const Reg16 limit, const Reg32 base) {
        volatile Reg8 aux[6];
        volatile Reg16 * l = reinterpret_cast<volatile Reg16 *>(&aux[0]);
        volatile Reg32 * b = reinterpret_cast<volatile Reg32 *>(&aux[2]);

        *l = limit;
        *b = base;
        ASM("lgdt %0" : : "m"(aux[0]));
    }

    static void idtr(Reg16 * limit, Reg32 * base) {
        volatile Reg8 aux[6];
        volatile Reg16 * l = reinterpret_cast<volatile Reg16 *>(&aux[0]);
        volatile Reg32 * b = reinterpret_cast<volatile Reg32 *>(&aux[2]);

        ASM("sidt %0" : "=m"(aux[0]) :);
        *limit = *l;
        *base = *b;
    }
    static void idtr(const Reg16 limit, const Reg32 base) {
        volatile Reg8 aux[6];
        volatile Reg16 * l = reinterpret_cast<volatile Reg16 *>(&aux[0]);
        volatile Reg32 * b = reinterpret_cast<volatile Reg32 *>(&aux[2]);

        *l = limit;
        *b = base;
        ASM("lidt %0" : : "m" (aux[0]));
    }

    static Reg16 cs() { Reg16 r; ASM("mov %%cs,%0" : "=r"(r) :); return r; }
    static Reg16 ds() { Reg16 r; ASM("mov %%ds,%0" : "=r"(r) :); return r; }
    static Reg16 es() { Reg16 r; ASM("mov %%es,%0" : "=r"(r) :); return r; }
    static Reg16 ss() { Reg16 r; ASM("mov %%ss,%0" : "=r"(r) :); return r; }
    static Reg16 fs() { Reg16 r; ASM("mov %%fs,%0" : "=r"(r) :); return r; }
    static Reg16 gs() { Reg16 r; ASM("mov %%gs,%0" : "=r"(r) :); return r; }

    static Reg16 tr() { Reg16 r; ASM("str %0" : "=r"(r) :); return r; }
    static void tr(Reg16 r) {    ASM("ltr %0" : : "r"(r)); }

    static void bts(Log_Addr addr, const int bit) { ASM("bts %1,%0" : "=m"(addr) : "r"(bit)); }
    static void btr(Log_Addr addr, const int bit) { ASM("btr %1,%0" : "=m"(addr) : "r"(bit)); }

    static int bsf(Log_Addr addr) { unsigned int pos;      ASM("bsf %1,%0"  : "=a"(pos) : "m"(addr) : ); return pos; }
    static int bsr(Log_Addr addr) { register int pos = -1; ASM("bsr %1, %0" : "=a"(pos) : "m"(addr) : ); return pos; }

    static Reg64 rdmsr(Reg32 msr) { Reg64 r; ASM("rdmsr" : "=A"(r) : "c"(msr)); return r; }
    static void wrmsr(Reg32 msr, Reg64 r) {  ASM("wrmsr" : : "c"(msr), "A"(r)); }

    static Reg8 in8(IO_Port p) { Reg8 r;    ASM("inb %1,%0" : "=a"(r) : "d"(p)); return r; }
    static Reg16 in16(IO_Port p) { Reg16 r; ASM("inw %1,%0" : "=a"(r) : "d"(p)); return r; }
    static Reg32 in32(IO_Port p) { Reg32 r; ASM("inl %1,%0" : "=a"(r) : "d"(p)); return r; }
    static void out8(IO_Port p, Reg8 r) {   ASM("outb %1,%0" : : "d"(p), "a"(r)); }
    static void out16(IO_Port p, Reg16 r) { ASM("outw %1,%0" : : "d"(p), "a"(r)); }
    static void out32(IO_Port p, Reg32 r) { ASM("outl %1,%0" : : "d"(p), "a"(r)); }

    static void switch_tss(Reg32 selector) {
        struct {
            Reg32 offset;
            Reg32 selector;
        } address;

        address.offset   = 0;
        address.selector = selector;

        ASM("ljmp *%0" : "=o" (address));
    }

    static unsigned int temperature() {
        Reg64 therm_read = rdmsr(THERM_STATUS);
        Reg64 temp_target_read = rdmsr(TEMPERATURE_TARGET);
        int bits = 22 - 16 + 1;
        therm_read >>= 16;
        therm_read &= (1ULL << bits) - 1;
        bits = 23 - 16 + 1;
        temp_target_read >>= 16;
        temp_target_read &= (1ULL << bits) - 1;
        return (temp_target_read - therm_read);
    }

private:
    template<typename Head, typename ... Tail>
    static void init_stack_helper(Log_Addr sp, Head head, Tail ... tail) {
        *static_cast<Head *>(sp) = head;
        init_stack_helper(sp + sizeof(Head *), tail ...);
    }
    static void init_stack_helper(Log_Addr sp) {}

public:
    static void smp_barrier_init(unsigned int cores);
private:
    static void init();

private:
    static volatile unsigned int _cores;
    static Hertz _cpu_clock;
    static Hertz _cpu_current_clock;
    static Hertz _bus_clock;
};

inline void CPU::Context::pop(bool interrupt)
{
    ASM("       popa                    # pop registers                         \n"
        "       iret                    # pop [SS, USP], FLAGS, CS, and IP,     \n"
        "                               #   and return 			        \n");
}

inline void CPU::Context::push(bool interrupt)
{
if(!interrupt) {
    ASM("       pop     %esi            # recover return address from the stack \n"
        "       pushf                   # create a stack structure for IRET     \n"
        "       push    %cs             #   with FLAGS, CS                      \n"
        "       push    %esi            #   and IP                              \n");
}
    ASM("       pusha                   # push registers                        \n");
}

inline CPU::Reg64 htole64(CPU::Reg64 v) { return CPU::htole64(v); }
inline CPU::Reg32 htole32(CPU::Reg32 v) { return CPU::htole32(v); }
inline CPU::Reg16 htole16(CPU::Reg16 v) { return CPU::htole16(v); }
inline CPU::Reg64 letoh64(CPU::Reg64 v) { return CPU::letoh64(v); }
inline CPU::Reg32 letoh32(CPU::Reg32 v) { return CPU::letoh32(v); }
inline CPU::Reg16 letoh16(CPU::Reg16 v) { return CPU::letoh16(v); }

inline CPU::Reg64 htobe64(CPU::Reg64 v) { return CPU::htobe64(v); }
inline CPU::Reg32 htobe32(CPU::Reg32 v) { return CPU::htobe32(v); }
inline CPU::Reg16 htobe16(CPU::Reg16 v) { return CPU::htobe16(v); }
inline CPU::Reg64 betoh64(CPU::Reg64 v) { return CPU::betoh64(v); }
inline CPU::Reg32 betoh32(CPU::Reg32 v) { return CPU::betoh32(v); }
inline CPU::Reg16 betoh16(CPU::Reg16 v) { return CPU::betoh16(v); }

inline CPU::Reg32 htonl(CPU::Reg32 v) { return CPU::htonl(v); }
inline CPU::Reg16 htons(CPU::Reg16 v) { return CPU::htons(v); }
inline CPU::Reg32 ntohl(CPU::Reg32 v) { return CPU::ntohl(v); }
inline CPU::Reg16 ntohs(CPU::Reg16 v) { return CPU::ntohs(v); }

__END_SYS

#endif
