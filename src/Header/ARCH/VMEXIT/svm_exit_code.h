#pragma once
#include "../../datatypes.h"

enum svm_exit_code : uint64_t {
    VMEXIT_CR0_READ = 0x0, // Read of CR 0 through 15, respectively.
    VMEXIT_CR1_READ = 0x1,
    VMEXIT_CR2_READ = 0x2,
    VMEXIT_CR3_READ = 0x3,
    VMEXIT_CR4_READ = 0x4,
    VMEXIT_CR5_READ = 0x5,
    VMEXIT_CR6_READ = 0x6,
    VMEXIT_CR7_READ = 0x7,
    VMEXIT_CR8_READ = 0x8,
    VMEXIT_CR9_READ = 0x9,
    VMEXIT_CR10_READ = 0xA,
    VMEXIT_CR11_READ = 0xB,
    VMEXIT_CR12_READ = 0xC,
    VMEXIT_CR13_READ = 0xD,
    VMEXIT_CR14_READ = 0xE,
    VMEXIT_CR15_READ = 0xF,

    VMEXIT_CR_WRITE = 0x10, // Write of CR 0 through 15, respectively.
    VMEXIT_CR0_WRITE = 0x10,
    VMEXIT_CR1_WRITE = 0x11,
    VMEXIT_CR2_WRITE = 0x12,
    VMEXIT_CR3_WRITE = 0x13,
    VMEXIT_CR4_WRITE = 0x14,
    VMEXIT_CR5_WRITE = 0x15,
    VMEXIT_CR6_WRITE = 0x16,
    VMEXIT_CR7_WRITE = 0x17,
    VMEXIT_CR8_WRITE = 0x18,
    VMEXIT_CR9_WRITE = 0x19,
    VMEXIT_CR10_WRITE = 0x1A,
    VMEXIT_CR11_WRITE = 0x1B,
    VMEXIT_CR12_WRITE = 0x1C,
    VMEXIT_CR13_WRITE = 0x1D,
    VMEXIT_CR14_WRITE = 0x1E,
    VMEXIT_CR15_WRITE = 0x1F,

    // DR read/write operations
    VMEXIT_DR0_READ = 0x20, // Read of DR 0 through 15, respectively.
    VMEXIT_DR1_READ = 0x21,
    VMEXIT_DR2_READ = 0x22,
    VMEXIT_DR3_READ = 0x23,
    VMEXIT_DR4_READ = 0x24,
    VMEXIT_DR5_READ = 0x25,
    VMEXIT_DR6_READ = 0x26,
    VMEXIT_DR7_READ = 0x27,
    VMEXIT_DR8_READ = 0x28,
    VMEXIT_DR9_READ = 0x29,
    VMEXIT_DR10_READ = 0x2A,
    VMEXIT_DR11_READ = 0x2B,
    VMEXIT_DR12_READ = 0x2C,
    VMEXIT_DR13_READ = 0x2D,
    VMEXIT_DR14_READ = 0x2E,
    VMEXIT_DR15_READ = 0x2F,

    VMEXIT_DR0_WRITE = 0x30, // Write of DR 0 through 15, respectively.
    VMEXIT_DR1_WRITE = 0x31,
    VMEXIT_DR2_WRITE = 0x32,
    VMEXIT_DR3_WRITE = 0x33,
    VMEXIT_DR4_WRITE = 0x34,
    VMEXIT_DR5_WRITE = 0x35,
    VMEXIT_DR6_WRITE = 0x36,
    VMEXIT_DR7_WRITE = 0x37,
    VMEXIT_DR8_WRITE = 0x38,
    VMEXIT_DR9_WRITE = 0x39,
    VMEXIT_DR10_WRITE = 0x3A,
    VMEXIT_DR11_WRITE = 0x3B,
    VMEXIT_DR12_WRITE = 0x3C,
    VMEXIT_DR13_WRITE = 0x3D,
    VMEXIT_DR14_WRITE = 0x3E,
    VMEXIT_DR15_WRITE = 0x3F,

    // Exception handling
    VMEXIT_EXCEPTION_0 = 0x40, // Exception vector 0–31, respectively.
    VMEXIT_EXCEPTION_1 = 0x41,
    VMEXIT_EXCEPTION_2 = 0x42,
    VMEXIT_EXCEPTION_3 = 0x43,
    VMEXIT_EXCEPTION_4 = 0x44,
    VMEXIT_EXCEPTION_5 = 0x45,
    VMEXIT_EXCEPTION_6 = 0x46,
    VMEXIT_EXCEPTION_7 = 0x47,
    VMEXIT_EXCEPTION_8 = 0x48,
    VMEXIT_EXCEPTION_9 = 0x49,
    VMEXIT_EXCEPTION_10 = 0x4A,
    VMEXIT_EXCEPTION_11 = 0x4B,
    VMEXIT_EXCEPTION_12 = 0x4C,
    VMEXIT_EXCEPTION_13 = 0x4D,
    VMEXIT_EXCEPTION_14 = 0x4E,
    VMEXIT_EXCEPTION_15 = 0x4F,
    VMEXIT_EXCEPTION_16 = 0x50,
    VMEXIT_EXCEPTION_17 = 0x51,
    VMEXIT_EXCEPTION_18 = 0x52,
    VMEXIT_EXCEPTION_19 = 0x53,
    VMEXIT_EXCEPTION_20 = 0x54,
    VMEXIT_EXCEPTION_21 = 0x55,
    VMEXIT_EXCEPTION_22 = 0x56,
    VMEXIT_EXCEPTION_23 = 0x57,
    VMEXIT_EXCEPTION_24 = 0x58,
    VMEXIT_EXCEPTION_25 = 0x59,
    VMEXIT_EXCEPTION_26 = 0x5A,
    VMEXIT_EXCEPTION_27 = 0x5B,
    VMEXIT_EXCEPTION_28 = 0x5C,
    VMEXIT_EXCEPTION_29 = 0x5D,
    VMEXIT_EXCEPTION_30 = 0x5E,
    VMEXIT_EXCEPTION_31 = 0x5F,

    // Physical interrupts
    VMEXIT_INTR = 0x60, // Physical INTR (maskable interrupt).
    VMEXIT_NMI = 0x61, // Physical NMI.
    VMEXIT_SMI = 0x62, // Physical SMI (additional info in EXITINFO1).
    VMEXIT_INIT = 0x63, // Physical INIT.
    VMEXIT_VINTR = 0x64, // Virtual INTR.

    // Control register operations
    VMEXIT_CR0_SEL_WRITE = 0x65, // Write of CR0 that changed any bits other than CR0.TS or CR0.MP.

    // Register reads
    VMEXIT_IDTR_READ = 0x66, // Read of IDTR.
    VMEXIT_GDTR_READ = 0x67, // Read of GDTR.
    VMEXIT_LDTR_READ = 0x68, // Read of LDTR.
    VMEXIT_TR_READ = 0x69, // Read of TR.

    // Register writes
    VMEXIT_IDTR_WRITE = 0x6A, // Write of IDTR.
    VMEXIT_GDTR_WRITE = 0x6B, // Write of GDTR.
    VMEXIT_LDTR_WRITE = 0x6C, // Write of LDTR.
    VMEXIT_TR_WRITE = 0x6D, // Write of TR.

    // Other instructions
    VMEXIT_RDTSC = 0x6E, // RDTSC instruction.
    VMEXIT_RDPMC = 0x6F, // RDPMC instruction.
    VMEXIT_PUSHF = 0x70, // PUSHF instruction.
    VMEXIT_POPF = 0x71, // POPF instruction.
    VMEXIT_CPUID = 0x72, // CPUID instruction.
    VMEXIT_RSM = 0x73, // RSM instruction.
    VMEXIT_IRET = 0x74, // IRET instruction.
    VMEXIT_SWINT = 0x75, // Software interrupt (INTn instructions).
    VMEXIT_INVD = 0x76, // INVD instruction.
    VMEXIT_PAUSE = 0x77, // PAUSE instruction.
    VMEXIT_HLT = 0x78, // HLT instruction.
    VMEXIT_INVLPG = 0x79, // INVLPG instruction.
    VMEXIT_INVLPGA = 0x7A, // INVLPGA instruction.
    VMEXIT_IOIO = 0x7B, // IN or OUT accessing protected port.
    VMEXIT_MSR = 0x7C, // RDMSR or WRMSR access to protected MSR.
    VMEXIT_TASK_SWITCH = 0x7D, // Task switch.
    VMEXIT_FERR_FREEZE = 0x7E, // FP legacy handling enabled, and processor is frozen in an x87/mmx instruction waiting for an interrupt.
    VMEXIT_SHUTDOWN = 0x7F, // Shutdown

    VMEXIT_VMRUN = 0x80, // VMRUN instruction.
    VMEXIT_VMMCALL = 0x81, // VMMCALL instruction.
    VMEXIT_VMLOAD = 0x82, // VMLOAD instruction.
    VMEXIT_VMSAVE = 0x83, // VMSAVE instruction.

    VMEXIT_STGI = 0x84, // STGI instruction.
    VMEXIT_CLGI = 0x85, // CLGI instruction.
    VMEXIT_SKINIT = 0x86, // SKINIT instruction.
    VMEXIT_RDTSCP = 0x87, // RDTSCP instruction.
    VMEXIT_ICEBP = 0x88, // ICEBP instruction.
    VMEXIT_WBINVD = 0x89, // WBINVD instruction.
    VMEXIT_MONITOR = 0x8A, // MONITOR instruction.
    VMEXIT_MWAIT = 0x8B, // MWAIT instruction.
    VMEXIT_MWAIT_CONDITIONAL = 0x8C, // MWAIT instruction with the events hint set in RCX.
    VMEXIT_RDPRU = 0x8E, // RDPRU instruction.
    VMEXIT_XSETBV = 0x8D, // XSETBV instruction.
    VMEXIT_EFER_WRITE_TRAP = 0x8F, // Write of EFER MSR (occurs after guest instruction finishes)

    VMEXIT_CR0_WRITE_TRAP = 0x90, // Write of CR0-15, respectively (occurs after guest instruction finishes)
    VMEXIT_CR1_WRITE_TRAP = 0x91,
    VMEXIT_CR2_WRITE_TRAP = 0x92,
    VMEXIT_CR3_WRITE_TRAP = 0x93,
    VMEXIT_CR4_WRITE_TRAP = 0x94,
    VMEXIT_CR5_WRITE_TRAP = 0x95,
    VMEXIT_CR6_WRITE_TRAP = 0x96,
    VMEXIT_CR7_WRITE_TRAP = 0x97,
    VMEXIT_CR8_WRITE_TRAP = 0x98,
    VMEXIT_CR9_WRITE_TRAP = 0x99,
    VMEXIT_CR10_WRITE_TRAP = 0x9A,
    VMEXIT_CR11_WRITE_TRAP = 0x9B,
    VMEXIT_CR12_WRITE_TRAP = 0x9C,
    VMEXIT_CR13_WRITE_TRAP = 0x9D,
    VMEXIT_CR14_WRITE_TRAP = 0x9E,
    VMEXIT_CR15_WRITE_TRAP = 0x9F,

    VMEXIT_INVLPGB = 0xA0, // INVLPG instruction.
    VMEXIT_INVLPGB_ILLEGAL = 0xA1, // INVLPG instruction with an illegal operand.
    VMEXIT_INVPCID = 0xA2, // INVPCID instruction.
    VMEXIT_MCOMMIT = 0xA3, // MCOMMIT instruction.
    VMEXIT_TLBSYNC = 0xA4, // TLBSYNC instruction.
    VMEXIT_BUSLOCK = 0xA5, // Bus lock while Bus Lock Threshold Counter value is 0
    
    VMEXIT_NPF = 0x400, // Nested paging: host-level page fault occurred (EXITINFO1 contains fault error code; EXITINFO2 contains the guest physical address causing the fault)
    VMEXIT_AVIC_INCOMPLETE_IPI = 0x401, // AVIC—Virtual IPI delivery not completed. 
    VMEXIT_AVIC_NOACCEL = 0x402, // AVIC—Attempted access by guest to vAPIC register not handled by AVIC hardware
    VMEXIT_VMGEXIT = 0x403, // VMGEXIT instruction

    VMEXIT_INVALID = -1, // Invalid guest state in VMCB
    VMEXIT_BUSY = -2, // BUSY bit was set in the encrypted VMSA
    VMEXIT_IDLE_REQUIRED = -3, // The sibling thread is not in an idle state
};