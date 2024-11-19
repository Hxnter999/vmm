#pragma once
#include <commons.h>

union exitinfo1_t
{
	uint64_t info;
	union {
		struct {  // 15.8.1 in the AMD manual
			uint64_t gpr_number : 4;
			uint64_t must_be_zero : 59;
			uint64_t mov_to_crx : 1; // undefined if cr0
			// MOV-to-CR0 Special Case. If the instruction is MOV-to-CR, the GPR number is provided; 
			// if the instruction is LMSW (Load Machine Status Word; lower 16 bits of cr0) or 
			// CLTS (Clear Task Switch; 4th bit of cr0), no additional information is provided and bit 63 is not set.
		} mov_cr;

		struct {
			uint64_t gpr_number : 4;
			uint64_t must_be_zero : 60;
		} mov_dr;

		struct {
			uint64_t software_interrupt_number : 8;
			uint64_t must_be_zero : 56;
		} int_n;

		struct {
			uint64_t linear_address : 64;
		} invlpg;

		struct { // 15.10.2
			uint64_t access_type : 1; // 0 = OUT | 1 = IN
			uint64_t reserved1 : 1;
			uint64_t string_based_port_access : 1; // string based port access (ins, outs)
			uint64_t repeated_port_access : 1;
			uint64_t size8 : 1; // 8 bit operand size
			uint64_t size16 : 1; // 16 bit operand size
			uint64_t size32 : 1; // 32 bit operand size
			uint64_t address_16 : 1; // 16 bit address size
			uint64_t address_32 : 1; // 32 bit address size
			uint64_t address_64 : 1; // 64 bit address size
			uint64_t effective_segment_number : 3;
			uint64_t reserved13 : 3;
			uint64_t port_number : 16;
			uint64_t unused : 32;
		} in_out;

		// If the SMI wasn't asserted during an I/O instruction, the extra EXITINFO1 and EXITINFO2 bits are undefined
		struct { // 15.10.3 and 15.13.3
			uint64_t smi_source : 1; // 0 = internal | 1 = external: whether the SMI was caused internally by I / O Trapping(=0), or asserted externally(=1).
			uint64_t redirect_machine_check : 1; // smi was due to a redirect machine check error
			uint64_t reserved2 : 8;
			uint64_t effective_segment_number : 3; // 15.10.3: On intercepted SMI-on-I/O, bits 12:10 of EXITINFO1 encode the segment. For definitions of the remaining bits of this field, (Section 15.13.3)
			uint64_t reserved13 : 19;
			uint64_t access_type : 1; // 0 = OUT | 1 = IN
			uint64_t valid : 1; // smi was detected during an I/O instruction
			uint64_t string_based_port_access : 1; // string based port access (ins, outs)
			uint64_t repeated_port_access : 1;
			uint64_t size8 : 1; // 8 bit operand size
			uint64_t size16 : 1; // 16 bit operand size
			uint64_t size32 : 1; // 32 bit operand size
			uint64_t address_16 : 1; // 16 bit address size
			uint64_t address_32 : 1; // 32 bit address size
			uint64_t address_64 : 1; // 64 bit address size
			uint64_t reserved42 : 1;
			uint64_t eflags_tf : 1; // eflags.tf value
			uint64_t breakpoint : 1; // I/O breakpoint matches
			uint64_t port_number : 16;
		} smi;

		struct { // 15.11
			uint64_t value : 64;
			inline bool is_read() { return (value == 0); }
			inline bool is_write() { return (value == 1); }
		} msr;

		// 15.12 Exception intercepts

		// #DE: EXITINFO1 & EXITINFO2 are undefined
		// #DB: EXITINFO1 & EXITINFO2 are undefined
		// Vector 2: Reserved and not implemented, use the NMI intercept instead. Effect of setting this bit is undefined
		// #BP: EXITINFO1 & EXITINFO2 are undefined. Cs:rIP are those of the INT3 instruction
		// #OF: EXITINFO1 & EXITINFO2 are undefined
		// #BR: EXITINFO1 & EXITINFO2 are undefined
		// #UD: EXITINFO1 & EXITINFO2 are undefined
		// #NM: EXITINFO1 & EXITINFO2 are undefined
		// #DF: EXITINFO1 & EXITINFO2 are undefined. rIP is undefined
		// Vector 9: Reserved and not implemented, use the #PF intercept instead. Effect of setting this bit is undefined

		// #TS : EXITINFO1 & EXITINFO2 are undefined. rIP may point to either the instruction causing the task switch or the first instruction of the incoming task
		struct { // 15.14
			uint64_t segment_selector : 16; // holds the segment selector identifying the incoming TSS.
			uint64_t reserved16 : 48;
		} task_switch;

		// #NP
		struct { // 15.15
			uint64_t error_code : 64; // the error code that would be pushed on the stack by a #NP exception. 
		} segment_not_present;

		// #SS
		struct { // 15.16
			uint64_t error_code : 64; // the error code that would be pushed on the stack by a #SS exception.
		} stack_segment;

		// #GP
		struct {
			uint64_t error_code : 64; // the error code that would be pushed on the stack by a #GP exception
		} general_protection;

		// #PF
		struct { // This intercept is tested before CR2 is written by the exception.
			uint64_t error_code : 64; // The error code saved in EXITINFO1 is the same as would be pushed onto the stack by a non - intercepted #PF exception in protected mode.
		} page_fault;

		// #NPF
		struct { // 15.25.6 (& 15.36.10 for SEV_SNP)
			uint64_t present : 1; // 0 = not present | 1 = present
			uint64_t rw : 1; // 1 = write access (e that host table walks for guest page tables are always treated as data writes.)
			uint64_t us : 1; // 1 = user access (npt accesses by MMU are treated as user access)
			uint64_t rsv : 1; // 1 = reserved bits were set
			uint64_t id : 1; // 1 = code read access
			uint64_t ss : 1; // 1 = shadow stack access 
			uint64_t sev_snp_reserved : 26; // used when secure nested paging is enabled
			uint64_t final_gpa_translation : 1; // 1 = occured while translating guest's final physical address
			uint64_t gpt_translation : 1; // 1 = occured while translating guest's guest page table
			uint64_t reserved34 : 3;
			uint64_t sss_page : 1; // supervisor shadow stack page in the leaf node of npt and shadow stack check is enabled
			uint64_t reserved38 : 26;
		} nested_page_fault;

		// #MF: This intercept is tested after the floating point status word has been written, as is the case for a normal FP exception.The EXITINFO1 and EXITINFO2 fields are undefined

		// #AC
		struct {
			uint64_t error_code : 64; // the error code that would be pushed on the stack by an #AC exception.
		} alignment_check;

		// #MC: The EXITINFO1 and EXITINFO2 fields are undefined

		// #XF: This intercept is tested after the SIMD status word (MXCSR) has been written, as is the case for a normal FP exception.The EXITINFO1 and EXITINFO2 fields are undefined

		// #SX 
		struct {
			uint64_t error_code : 64; // the error code that would be pushed on the stack by a #SX exception.
		} security_exception;

		// #CP
		struct {
			uint64_t error_code : 64; // the error code that would be pushed on the stack by a #CP exception.
		} control_protection;

		// IPI 15.29.9.1
		struct {
			uint64_t icrl : 32; // Value written to the vAPIC ICRL register.
			uint64_t icrh : 32; // Value written to the vAPIC ICRH register.
		} ipi;

		// 15.29.9.2
		struct {
			uint64_t reserved : 4;
			uint64_t apic_offset : 8;
			uint64_t reserved12 : 20;
			uint64_t write : 1;
			uint64_t reserved33 : 31;
		} vapic;
	};
}; // union exitinfo1