//// EPOS Raspberry Pi3 (Cortex-A53) Initialization
//
//#include <system/config.h>
//#include <machine.h>
//#include <architecture/cpu.h>
//#include __MODEL_H
//
//__BEGIN_SYS
//

// static void branch_prediction_enable() {
//     ASM("                                                                           \t\n\
//         MRC     p15, 0, r0, c1, c0, 0                  // Read SCTLR                \t\n\
//         ORR     r0, r0, #(1 << 11)                     // Set the Z bit (bit 11)    \t\n\
//         MCR     p15, 0,r0, c1, c0, 0                   // Write SCTLR               \t\n\
//         ");
// }

// static void enable_dside_prefetch() {
//     ASM("MRC p15, 0, r0, c1, c0, 1 \t\n\
//          ORR r0, r0, #(0x1 << 2)   \t\n\
//          MCR p15, 0, r0, c1, c0, 1 ");
// }

// static void invalidate_tlb() {
//     ASM("MOV r0, #0x0 \t\n MCR p15, 0, r0, c8, c7, 0"); // TLBIALL - Invalidate entire Unifed TLB
// }

// static void clear_branch_prediction_array() {
//     ASM("MOV r0, #0x0 \t\n MCR p15, 0, r0, c7, c5, 6"); // BPIALL - Invalidate entire branch predictor array
// }

// static void set_domain_access() {
//     // ASM("MCR p15, 0, %0, c3, c0, 0" : : "p"(Realview_PBX::TTBCR_DOMAIN) :);
//     ASM("MCR p15, 0, %0, c3, c0, 0" : : "p"(0xFFFFFFFF) :);
// }

// static void enable_scu() {
//     Machine::scu(0) |= 0x1;
// }

// static void secure_scu_invalidate() {
//     // void secure_SCU_invalidate(unsigned int cpu, unsigned int ways)
//     // cpu: 0x0=CPU 0 0x1=CPU 1 etc...
//     // This function invalidates the SCU copy of the tag rams
//     // for the specified core.  Typically only done at start-up.
//     // Possible flow:
//     //- Invalidate L1 caches
//     //- Invalidate SCU copy of TAG RAMs
//     //- Join SMP
//     Reg32 _cpu = Machine::cpu_id();
//     _cpu = _cpu << 2; // Convert into bit offset (four bits per core)
//     Reg32 _ways = 0x0F; // all four ways
//     _ways = _ways << _cpu; // Shift ways into the correct CPU field
//     Machine::scu(SCU_IARSS) = _ways;
// }

// static void scu_enable_cache_coherence() {
//     Machine::scu(SCU_CONFIG) |= 0xF0;
// }

// static void enable_maintenance_broadcast() {
//     // Enable the broadcasting of cache & TLB maintenance operations
//     // When enabled AND in SMP, broadcast all "inner sharable"
//     // cache and TLM maintenance operations to other SMP cores
//     ASM("MRC     p15, 0, r0, c1, c0, 1   // Read ACTLR.             \t\n\
//          ORR     r0, r0, #(0x01)         // Set the FW bit (bit 0). \t\n\
//          MCR     p15, 0, r0, c1, c0, 1   // Write ACTLR."
//     );
// }

// static void join_smp() {
//     ASM("                                                                       \t\n\
//         MRC     p15, 0, r0, c1, c0, 1   // Read ACTLR                           \t\n\
//         ORR     r0, r0, #(0x01 << 6)    // Set SMP bit                          \t\n\
//         MCR     p15, 0, r0, c1, c0, 1   // Write ACTLR                          \t\n\
//         ");
// }

// static void page_tables_setup() {
//     Reg32 aux = 0x0;
//     for (int curr_page = 1006; curr_page >= 0; curr_page--) {
//         // aux = TTB_DESCRIPTOR | (curr_page << 20);
//         aux = 0x90C0E | (curr_page << 20);
//         reinterpret_cast<volatile Reg32 *>(Traits<Machine>::PAGE_TABLES)[curr_page] = aux;
//     }
//     aux = 0x90C0A | (1007 << 20);
//     reinterpret_cast<volatile Reg32 *>(Traits<Machine>::PAGE_TABLES)[1007] = aux;
//     for (int curr_page = 4095; curr_page > 1007; curr_page--) {
//         aux = 0x90C16 | (curr_page << 20);
//         reinterpret_cast<volatile Reg32 *>(Traits<Machine>::PAGE_TABLES)[curr_page] = aux;
//     }
//     // reinterpret_cast<volatile Reg32 *>(Traits<Machine>::PAGE_TABLES)[0] = TTB_DESCRIPTOR;
// }

// static void enable_mmu() {
//     // TTB0 size is 16 kb, there is no TTB1 and no TTBCR
//     // ARMv7 Architecture Reference Manual, pages 1330
//     ASM ("MOV r0, #0x0 \t\n MCR p15, 0, r0, c2, c0, 2"); // Write Translation Table Base Control Register.
//     ASM ("MCR p15, 0, %0, c2, c0, 0" : : "p"(Traits<Machine>::PAGE_TABLES) :); // Write Translation Table Base Register 0.

//     // Enable MMU
//     //-------------
//     //0     - M, set to enable MMU
//     // Leaving the caches disabled until after scatter loading.
//     ASM ("                                                                      \t\n\
//         MRC     p15, 0, r0, c1, c0, 1       // Read ACTLR                       \t\n\
//         ORR     r0, r0, #(0x01 << 6)        // Set SMP bit                      \t\n\
//         MCR     p15, 0, r0, c1, c0, 1       // Write ACTLR                      \t\n\
//         MRC     p15, 0, r0, c1, c0, 0       // Read current control reg         \t\n\
//         ORR     r0, r0, #(0x1 << 2)         // The C bit (data cache).          \t\n\
//         BIC     R0, R0, #(0x1 << 29)        // Set AFE to 0 disable Access Flag.\t\n\
//         ORR     r0, r0, #(0x1 << 12)        // The I bit (instruction cache).   \t\n\
//         ORR     r0, r0, #0x01               // Set M bit                        \t\n\
//         MCR     p15, 0, r0, c1, c0, 0       // Write reg back                   \t\n\
//         ");
// }

// static void clear_bss() {
//     Reg32 bss_start, bss_end;
//     ASM("LDR %0, =__bss_start__" : "=r"(bss_start) :);
//     ASM("LDR %0, =__bss_end__" : "=r"(bss_end) :);
//     Reg32 limit = (bss_end - bss_start)/4;
//     for(Reg32 i = 0; i < limit; i++) {
//         reinterpret_cast<volatile Reg32 *>(bss_start)[i] = 0x0;
//     }
// }
//
//// DSB causes completion of all cache maintenance operations appearing in program
//// order before the DSB instruction.
//void dsb()
//{
//    ASM("dsb");
//}
//
//
//
//// An ISB instruction causes the effect of all branch predictor maintenance
//// operations before the ISB instruction to be visible to all instructions
//// after the ISB instruction.
// void isb()
// {
//    ASM("isb");
// }
//
//void invalidate_caches()
//{
/*
    ASM("                  \t\n\
// Disable L1 Caches.                                                                   \t\n\
    MRC P15, 0, R1, C1, C0, 0 // Read SCTLR.                                            \t\n\
    BIC R1, R1, #(0x1 << 2) // Disable D Cache.                                         \t\n\
    MCR P15, 0, R1, C1, C0, 0 // Write SCTLR.                                           \t\n\
                                                                                        \t\n\
    // Invalidate Data cache to create general-purpose code. Calculate there            \t\n\
    // cache size first and loop through each set + way.                                \t\n\
    MOV R0, #0x0 // R0 = 0x0 for L1 dcache 0x2 for L2 dcache.                           \t\n\
    MCR P15, 2, R0, C0, C0, 0 // CSSELR Cache Size Selection Register.                  \t\n\
    MRC P15, 1, R4, C0, C0, 0 // CCSIDR read Cache Size.                                \t\n\
    AND R1, R4, #0x7                                                                    \t\n\
    ADD R1, R1, #0x4 // R1 = Cache Line Size.                                           \t\n\
    LDR R3, =0x7FFF                                                                     \t\n\
    AND R2, R3, R4, LSR #13 // R2 = Cache Set Number – 1.                               \t\n\
    LDR R3, =0x3FF                                                                      \t\n\
    AND R3, R3, R4, LSR #3 // R3 = Cache Associativity Number – 1.                      \t\n\
    CLZ R4, R3 // R4 = way position in CISW instruction.                                \t\n\
    MOV R5, #0 // R5 = way loop counter.                                                \t\n\
way_loop:                                                                               \t\n\
    MOV R6, #0 // R6 = set loop counter.                                                \t\n\
set_loop:                                                                               \t\n\
    ORR R7, R0, R5, LSL R4 // Set way.                                                  \t\n\
    ORR R7, R7, R6, LSL R1 // Set set.                                                  \t\n\
    MCR P15, 0, R7, C7, C6, 2 // DCCISW R7.                                             \t\n\
    ADD R6, R6, #1 // Increment set counter.                                            \t\n\
    CMP R6, R2 // Last set reached yet?                                                 \t\n\
    BLE set_loop // If not, iterate set_loop,                                           \t\n\
    ADD R5, R5, #1 // else, next way.                                                   \t\n\
    CMP R5, R3 // Last way reached yet?                                                 \t\n\
    BLE way_loop // if not, iterate way_loop.                                           \t\n\
    // mov r2, #0                                                                          \t\n\
    // mcr p15, 0, r2, c7, c7, 0                                                           \t\n\
    ");
*/
//}
//


//void Raspberry_Pi3::pre_init()
//{
//    // Relocated the vector table
//    // ASM("MCR p15, 0, %0, c12, c0, 0" : : "p"(Traits<Machine>::VECTOR_TABLE) :);
//    ASM ("MOV R0, #0 \t\n MSR SPSR, R0 \t\n CPSIE aif");
//    // MMU init
//    invalidate_caches();
//    clear_branch_prediction_array();
//    invalidate_tlb();
//    enable_dside_prefetch();
//    set_domain_access();
//    dsb();
//    isb();
//
//    // Initialize PageTable.
//
//    // Create a basic L1 page table in RAM, with 1MB sections containing a flat
//    // (VA=PA) mapping, all pages Full Access, Strongly Ordered.
//
//    // It would be faster to create this in a read-only section in an assembly file.
//
//    if(CPU::id() == 0)
//        page_tables_setup();
//
//    // Activate the MMU
//    enable_mmu();
//    dsb();
//    isb();
//
//    // MMU now enable - Virtual address system now active
//
//    // Branch Prediction init
//    branch_prediction_enable();
//
//    // SMP initialization
//    if(CPU::id() == 0) {
//        //primary core
//
//        // gic enable is now on Machine::pre_init()
//
//        // FIXME:: is the following part of memory map or is there really a flat segment register?
//        // this is only a flat segment register that allows mapping the start point for the secondary cores
//        static const unsigned int FLAG_SET_REG = 0x40000000;
//        // set flag register to the address secondary CPUS are meant to start executing
//// /*
//        ASM("str %0, [%1, #0x9c]" : : "p"(Traits<Machine>::VECTOR_TABLE), "p"(FLAG_SET_REG) :);
//        ASM("str %0, [%1, #0xac]" : : "p"(Traits<Machine>::VECTOR_TABLE), "p"(FLAG_SET_REG) :);
//        ASM("str %0, [%1, #0xbc]" : : "p"(Traits<Machine>::VECTOR_TABLE), "p"(FLAG_SET_REG) :);
////*/
//        dsb();
//        // secondary cores reset is now on Machine::pre_init()
//        ASM ("SEV");
//
//        clear_bss();
//    } else {
//        //clear_interrupt();
//    }
//}
//
//void Raspberry_Pi3::init()
//{
//}
//
//__END_SYS
