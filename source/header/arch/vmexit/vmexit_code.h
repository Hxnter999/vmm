#pragma once
#include <commons.h>

enum class ipi_type : uint64_t
{
    invalid_interrupt_type = 0,
    ipi_target_not_running,
    invalid_target_in_ipi,
    invalid_backing_page_pointer,
    invalid_ipi_vector
};

enum class vmexit_code : uint64_t {
    CR0_READ = 0x0, // Read of CR 0 through 15, respectively.
    CR1_READ = 0x1,
    CR2_READ = 0x2,
    CR3_READ = 0x3,
    CR4_READ = 0x4,
    CR5_READ = 0x5,
    CR6_READ = 0x6,
    CR7_READ = 0x7,
    CR8_READ = 0x8,
    CR9_READ = 0x9,
    CR10_READ = 0xA,
    CR11_READ = 0xB,
    CR12_READ = 0xC,
    CR13_READ = 0xD,
    CR14_READ = 0xE,
    CR15_READ = 0xF,

    CR_WRITE = 0x10, // Write of CR 0 through 15, respectively.
    CR0_WRITE = 0x10,
    CR1_WRITE = 0x11,
    CR2_WRITE = 0x12,
    CR3_WRITE = 0x13,
    CR4_WRITE = 0x14,
    CR5_WRITE = 0x15,
    CR6_WRITE = 0x16,
    CR7_WRITE = 0x17,
    CR8_WRITE = 0x18,
    CR9_WRITE = 0x19,
    CR10_WRITE = 0x1A,
    CR11_WRITE = 0x1B,
    CR12_WRITE = 0x1C,
    CR13_WRITE = 0x1D,
    CR14_WRITE = 0x1E,
    CR15_WRITE = 0x1F,

    // DR read/write operations
    DR0_READ = 0x20, // Read of DR 0 through 15, respectively.
    DR1_READ = 0x21,
    DR2_READ = 0x22,
    DR3_READ = 0x23,
    DR4_READ = 0x24,
    DR5_READ = 0x25,
    DR6_READ = 0x26,
    DR7_READ = 0x27,
    DR8_READ = 0x28,
    DR9_READ = 0x29,
    DR10_READ = 0x2A,
    DR11_READ = 0x2B,
    DR12_READ = 0x2C,
    DR13_READ = 0x2D,
    DR14_READ = 0x2E,
    DR15_READ = 0x2F,

    DR0_WRITE = 0x30, // Write of DR 0 through 15, respectively.
    DR1_WRITE = 0x31,
    DR2_WRITE = 0x32,
    DR3_WRITE = 0x33,
    DR4_WRITE = 0x34,
    DR5_WRITE = 0x35,
    DR6_WRITE = 0x36,
    DR7_WRITE = 0x37,
    DR8_WRITE = 0x38,
    DR9_WRITE = 0x39,
    DR10_WRITE = 0x3A,
    DR11_WRITE = 0x3B,
    DR12_WRITE = 0x3C,
    DR13_WRITE = 0x3D,
    DR14_WRITE = 0x3E,
    DR15_WRITE = 0x3F,

    // Exception handling
    EXCEPTION_0 = 0x40, // Exception vector 0–31, respectively.
    DE = 0x40, // divide by zero

    EXCEPTION_1 = 0x41,
    DB = 0x41, // debug exception

    EXCEPTION_2 = 0x42,
    // NMI = 0x42, interception of this vector isnt supported, refer to chapter 15.12

    EXCEPTION_3 = 0x43,
    BP = 0x43, // breakpoint

    EXCEPTION_4 = 0x44,
    OF = 0x44, // overflow

    EXCEPTION_5 = 0x45,
    BR = 0x45, // bound range exceeded

    EXCEPTION_6 = 0x46,
    UD = 0x46, // invalid opcode

    EXCEPTION_7 = 0x47,
    NM = 0x47, // device not available

    EXCEPTION_8 = 0x48,
    DF = 0x48, // double fault

    EXCEPTION_9 = 0x49,
    CSO = 0x49, // coprocessor segment overrun

    EXCEPTION_10 = 0x4A,
    TS = 0x4A, // invalid TSS

    EXCEPTION_11 = 0x4B,
    NP = 0x4B, // segment not present

    EXCEPTION_12 = 0x4C,
    SS = 0x4C, // stack exception

    EXCEPTION_13 = 0x4D,
    GP = 0x4D, // general protection

    EXCEPTION_14 = 0x4E,
    PF = 0x4E, // page fault

    EXCEPTION_15 = 0x4F,

    EXCEPTION_16 = 0x50,
    MF = 0x50, // x87 FPU floating-point error

    EXCEPTION_17 = 0x51,
    AC = 0x51, // alignment check

    EXCEPTION_18 = 0x52,
    MC = 0x52, // machine check

    EXCEPTION_19 = 0x53,
    XF = 0x53, // SIMD floating-point exception

    EXCEPTION_20 = 0x54,

    EXCEPTION_21 = 0x55,
    CP = 0x55, // control protection exception

    EXCEPTION_22 = 0x56,
    EXCEPTION_23 = 0x57,
    EXCEPTION_24 = 0x58,
    EXCEPTION_25 = 0x59,
    EXCEPTION_26 = 0x5A,
    EXCEPTION_27 = 0x5B,

    EXCEPTION_28 = 0x5C,
    HV = 0x5C, // event injection exception

    EXCEPTION_29 = 0x5D,
    VC = 0x5D, // vmm communication exception

    EXCEPTION_30 = 0x5E,
    SX = 0x5E, // security exception

    EXCEPTION_31 = 0x5F,

    // Physical interrupts
    INTR = 0x60, // Physical INTR (maskable interrupt).
    NMI = 0x61, // Physical NMI.
    SMI = 0x62, // Physical SMI (additional info in EXITINFO1).
    INIT = 0x63, // Physical INIT.
    VINTR = 0x64, // Virtual INTR.

    // Control register operations
    CR0_SEL_WRITE = 0x65, // Write of CR0 that changed any bits other than CR0.TS or CR0.MP.

    // Register reads
    IDTR_READ = 0x66, // Read of IDTR.
    GDTR_READ = 0x67, // Read of GDTR.
    LDTR_READ = 0x68, // Read of LDTR.
    TR_READ = 0x69, // Read of TR.

    // Register writes
    IDTR_WRITE = 0x6A, // Write of IDTR.
    GDTR_WRITE = 0x6B, // Write of GDTR.
    LDTR_WRITE = 0x6C, // Write of LDTR.
    TR_WRITE = 0x6D, // Write of TR.

    // Other instructions
    RDTSC = 0x6E, // RDTSC instruction.
    RDPMC = 0x6F, // RDPMC instruction.
    PUSHF = 0x70, // PUSHF instruction.
    POPF = 0x71, // POPF instruction.
    cpuid = 0x72, // CPUID instruction.
    RSM = 0x73, // RSM instruction.
    IRET = 0x74, // IRET instruction.
    SWINT = 0x75, // Software interrupt (INTn instructions).
    INVD = 0x76, // INVD instruction.
    PAUSE = 0x77, // PAUSE instruction.
    HLT = 0x78, // HLT instruction.
    INVLPG = 0x79, // INVLPG instruction.
    INVLPGA = 0x7A, // INVLPGA instruction.
    IOIO = 0x7B, // IN or OUT accessing protected port.
    msr = 0x7C, // RDMSR or WRMSR access to protected MSR.
    TASK_SWITCH = 0x7D, // Task switch.
    FERR_FREEZE = 0x7E, // FP legacy handling enabled, and processor is frozen in an x87/mmx instruction waiting for an interrupt.
    SHUTDOWN = 0x7F, // Shutdown

    VMRUN = 0x80, // VMRUN instruction.
    VMMCALL = 0x81, // VMMCALL instruction.
    VMLOAD = 0x82, // VMLOAD instruction.
    VMSAVE = 0x83, // VMSAVE instruction.
    STGI = 0x84, // STGI instruction.
    CLGI = 0x85, // CLGI instruction.
    SKINIT = 0x86, // SKINIT instruction.

    RDTSCP = 0x87, // RDTSCP instruction.
    ICEBP = 0x88, // ICEBP instruction.
    WBINVD = 0x89, // WBINVD instruction.
    MONITOR = 0x8A, // MONITOR instruction.
    MWAIT = 0x8B, // MWAIT instruction.
    MWAIT_CONDITIONAL = 0x8C, // MWAIT instruction with the events hint set in RCX.
    RDPRU = 0x8E, // RDPRU instruction.
    XSETBV = 0x8D, // XSETBV instruction.

    EFER_WRITE_TRAP = 0x8F, // Write of EFER MSR (occurs after guest instruction finishes)
    CR0_WRITE_TRAP = 0x90, // Write of CR0-15, respectively (occurs after guest instruction finishes)
    CR1_WRITE_TRAP = 0x91,
    CR2_WRITE_TRAP = 0x92,
    CR3_WRITE_TRAP = 0x93,
    CR4_WRITE_TRAP = 0x94,
    CR5_WRITE_TRAP = 0x95,
    CR6_WRITE_TRAP = 0x96,
    CR7_WRITE_TRAP = 0x97,
    CR8_WRITE_TRAP = 0x98,
    CR9_WRITE_TRAP = 0x99,
    CR10_WRITE_TRAP = 0x9A,
    CR11_WRITE_TRAP = 0x9B,
    CR12_WRITE_TRAP = 0x9C,
    CR13_WRITE_TRAP = 0x9D,
    CR14_WRITE_TRAP = 0x9E,
    CR15_WRITE_TRAP = 0x9F,

    INVLPGB = 0xA0, // INVLPG instruction.
    INVLPGB_ILLEGAL = 0xA1, // INVLPG instruction with an illegal operand.
    INVPCID = 0xA2, // INVPCID instruction.
    MCOMMIT = 0xA3, // MCOMMIT instruction.
    TLBSYNC = 0xA4, // TLBSYNC instruction.
    BUSLOCK = 0xA5, // Bus lock while Bus Lock Threshold Counter value is 0
    
    NPF = 0x400, // Nested paging: host-level page fault occurred (EXITINFO1 contains fault error code; EXITINFO2 contains the guest physical address causing the fault)
    AVIC_INCOMPLETE_IPI = 0x401, // AVIC—Virtual IPI delivery not completed. 
    AVIC_NOACCEL = 0x402, // AVIC—Attempted access by guest to vAPIC register not handled by AVIC hardware
    VMGEXIT = 0x403, // VMGEXIT instruction

    INVALID = -1, // Invalid guest state in VMCB
    BUSY = -2, // BUSY bit was set in the encrypted VMSA
    IDLE_REQUIRED = -3, // The sibling thread is not in an idle state
    INVALID_PMC = -4, // Invalid PMC state
};