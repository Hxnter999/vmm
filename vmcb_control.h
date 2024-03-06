#pragma once
#include "datatypes.h"
#include "exit_int_info.h"
#include "exitinfo1.h"

struct vmcb_control {
	union {
		uint32_t value0;
		struct {
			uint32_t cr0_read : 1;
			uint32_t cr1_read : 1;
			uint32_t cr2_read : 1;
			uint32_t cr3_read : 1;
			uint32_t cr4_read : 1;
			uint32_t cr5_read : 1;
			uint32_t cr6_read : 1;
			uint32_t cr7_read : 1;
			uint32_t cr8_read : 1;
			uint32_t cr9_read : 1;
			uint32_t cr10_read : 1;
			uint32_t cr11_read : 1;
			uint32_t cr12_read : 1;
			uint32_t cr13_read : 1;
			uint32_t cr14_read : 1;
			uint32_t cr15_read : 1;
			// ******************* 
			uint32_t cr0_write : 1;
			uint32_t cr1_write : 1;
			uint32_t cr2_write : 1;
			uint32_t cr3_write : 1;
			uint32_t cr4_write : 1;
			uint32_t cr5_write : 1;
			uint32_t cr6_write : 1;
			uint32_t cr7_write : 1;
			uint32_t cr8_write : 1;
			uint32_t cr9_write : 1;
			uint32_t cr10_write : 1;
			uint32_t cr11_write : 1;
			uint32_t cr12_write : 1;
			uint32_t cr13_write : 1;
			uint32_t cr14_write : 1;
			uint32_t cr15_write : 1;
		};
	};

	// 004h 
	union {
		uint32_t value4;
		struct {
			uint32_t dr0_read : 1;
			uint32_t dr1_read : 1;
			uint32_t dr2_read : 1;
			uint32_t dr3_read : 1;
			uint32_t dr4_read : 1;
			uint32_t dr5_read : 1;
			uint32_t dr6_read : 1;
			uint32_t dr7_read : 1;
			uint32_t dr8_read : 1;
			uint32_t dr9_read : 1;
			uint32_t dr10_read : 1;
			uint32_t dr11_read : 1;
			uint32_t dr12_read : 1;
			uint32_t dr13_read : 1;
			uint32_t dr14_read : 1;
			uint32_t dr15_read : 1;
			// ******************* 
			uint32_t dr0_write : 1;
			uint32_t dr1_write : 1;
			uint32_t dr2_write : 1;
			uint32_t dr3_write : 1;
			uint32_t dr4_write : 1;
			uint32_t dr5_write : 1;
			uint32_t dr6_write : 1;
			uint32_t dr7_write : 1;
			uint32_t dr8_write : 1;
			uint32_t dr9_write : 1;
			uint32_t dr10_write : 1;
			uint32_t dr11_write : 1;
			uint32_t dr12_write : 1;
			uint32_t dr13_write : 1;
			uint32_t dr14_write : 1;
			uint32_t dr15_write : 1;
		};
	};

	// 008h 
	union {
		uint32_t value8;
		struct { // Exception vector
			uint32_t ev0 : 1;
			uint32_t ev1 : 1;
			uint32_t ev2 : 1;
			uint32_t ev3 : 1;
			uint32_t ev4 : 1;
			uint32_t ev5 : 1;
			uint32_t ev6 : 1;
			uint32_t ev7 : 1;
			uint32_t ev8 : 1;
			uint32_t ev9 : 1;
			uint32_t ev10 : 1;
			uint32_t ev11 : 1;
			uint32_t ev12 : 1;
			uint32_t ev13 : 1;
			uint32_t ev14 : 1;
			uint32_t ev15 : 1;
			uint32_t ev16 : 1;
			uint32_t ev17 : 1;
			uint32_t ev18 : 1;
			uint32_t ev19 : 1;
			uint32_t ev20 : 1;
			uint32_t ev21 : 1;
			uint32_t ev22 : 1;
			uint32_t ev23 : 1;
			uint32_t ev24 : 1;
			uint32_t ev25 : 1;
			uint32_t ev26 : 1;
			uint32_t ev27 : 1;
			uint32_t ev28 : 1;
			uint32_t ev29 : 1;
			uint32_t ev30 : 1;
			uint32_t ev31 : 1;
		};
	};

	// 00ch 
	union {
		uint32_t valuec;
		struct {
			uint32_t intr : 1;	// intercept intr (physical maskable interrupt)
			uint32_t nmi : 1;	// intercept nmi (physical non-maskable interrupt)
			uint32_t smi : 1;	// intercept smi (physical system management interrupt)
			uint32_t init : 1;
			uint32_t vintr : 1;	// intercept virtual interrupt
			uint32_t cr0_ts_mp : 1;	// intercept cr0 writes that change bits other than cr0.ts or cr0.mp
			uint32_t read_idtr : 1;
			uint32_t read_gdtr : 1;
			uint32_t read_ldtr : 1;
			uint32_t read_tr : 1;
			uint32_t write_idtr : 1;
			uint32_t write_gdtr : 1;
			uint32_t write_ldtr : 1;
			uint32_t write_tr : 1;
			uint32_t rdtsc : 1;
			uint32_t rdpmc : 1;
			uint32_t pushf : 1;
			uint32_t popf : 1;
			uint32_t cpuid : 1;
			uint32_t rsm : 1;
			uint32_t iret : 1;
			uint32_t intn : 1;
			uint32_t invd : 1;
			uint32_t pause : 1;
			uint32_t hlt : 1;
			uint32_t invlpg : 1;
			uint32_t invlpga : 1;
			uint32_t ioio_prot : 1;	// ioio_prot: intercept in/out accesses to selected ports
			uint32_t msr_prot : 1;	// msr_prot—intercept rdmsr or wrmsr accesses to selected msr
			uint32_t task_switch : 1;
			uint32_t ferr_freeze : 1;	// ferr_freeze: intercept processor “freezing” during legacy ferr handling
			uint32_t shutdown : 1;	// shutdown intercepts
		};
	};

	// 010h 
	union {
		uint32_t value10;
		struct {
			uint32_t vmrun : 1;
			uint32_t vmmcall : 1;
			uint32_t vmmload : 1;
			uint32_t vmmsave : 1;
			uint32_t stgi : 1;
			uint32_t clgi : 1;
			uint32_t skinit : 1;
			uint32_t rdtscp : 1;
			uint32_t icebp : 1;
			uint32_t wbinvd : 1; // intercept wbinvd and wbnoinvd instruction
			uint32_t monitor : 1; // intercept monitor/monitorx instruction
			uint32_t mwait : 1; // intercept mwait/mwaitx instruction unconditionally
			uint32_t mwait_cond : 1; // intercept mwait/mwaitx instruction if monitor hardware is armed
			uint32_t xsetbv : 1; // intercept xsetbv instruction
			uint32_t rdpru : 1; // intercept rdpru instruction
			uint32_t write_effer : 1; // intercept writes of efer (occurs after guest instruction finishes
			uint32_t write_cr0_15 : 16; // intercept writes of cr0-15 (occurs after guest instruction finishes
		};
	};

	// 014h 
	union {
		uint32_t value14;
		struct {
			uint32_t invlpgb : 1;
			uint32_t invvpgb_illegal : 1;
			uint32_t invpcid : 1;
			uint32_t mcommit : 1;
			uint32_t tlbsync : 1; // intercept tlbsync instruction. presence of this bit is indicated by cpuid fn8000_000a, edx[24] = 1.
			uint32_t bus_lock : 1; // intercept bus lock operations when bus lock threshold
			uint32_t reserved6 : 26;
		};
	};

	// 018h - 030h  
	uint64_t reserved18[4];
	// 038h - 03bh 
	uint8_t reserved38[3];

	// 03ch 
	uint16_t pause_filter_threshold;

	// 03eh 
	uint16_t pause_filter_count;

	// 040h 
	union {
		uint64_t value40;
		struct {
			uint64_t ignored : 12; // ignored
			uint64_t iopm_base_pa : 52; // iopm_base_pa: i/o permission bit map base physical address
		};
	};

	// 048h 
	union {
		uint64_t value48;
		struct {
			uint64_t ignored : 12; // ignored
			uint64_t msrpm_base_pa : 52; // msrpm_base_pa: msr permission bit map base physical address
		};
	};

	// 050h 
	uint64_t tsc_offset;

	// 058h 
	union {
		uint64_t value58;
		struct {
			uint64_t guest_asid : 32; // guest asid
			// tlb_control: tlb control bits
			uint64_t do_nothing : 1; // do nothing
			uint64_t flush_entire_tlb : 2; // flush entire tlb (all entries, all asids) on vmrun should only be used by legacy hypervisor
			uint64_t flush_guest_tlb : 2; // flush tlb entries associated with guest asid on vmrun
			uint64_t flush_guest_non_global_tlb : 3; // flush this guest’s non-global tlb entries
			uint64_t reserved40 : 24;
		};
	};

	// 060h 
	union {
		uint64_t value60;
		struct {
			uint64_t v_tpr : 8;		// this value is written back to the vmcb at #vmexit.
			uint64_t v_irq : 1;		// this value is written back to the vmcb at #vmexit. this field is ignored on vmrun when avic is enabled
			uint64_t vgif : 1;		// vgif value (0 – virtual interrupts are masked, 1 – virtual interrupts are unmasked)
			uint64_t int_shadow : 1;
			uint64_t v_nmi : 1;		// indicates whether a virtual nmi is pending in the guest. the processor will clear v_nmi once it takes the virtual nmi
			uint64_t v_nmi_mask : 1;
			uint64_t reserved13 : 3;
			uint64_t v_ign_prio : 4;
			uint64_t v_ign_tpr : 1;
			uint64_t reserved21 : 5;
			uint64_t v_nmi_enable : 1;
			uint64_t reserved27 : 5;
			uint64_t v_intr_vector : 8;
			uint64_t reserved40 : 23;
			uint64_t busy : 1;
		};
	};

	// 068h
	union {
		uint64_t value68;
		struct {
			uint64_t interrupt_shadow : 1;		// interrupt_shadow - guest is in an interrupt shadow
			uint64_t guest_interrupt_mask : 1;	// guest_interrupt_mask - value of the rflags.if bit for the guest note : this value is written back to the vmcb on #vmexit.it is not used during vmru
			uint64_t reserved : 62;				// reserved
		};
	};

	// 070h
	uint64_t exit_code;

	// 078h
	EXITINFO1::exitinfo1 exit_info_1;

	// 080h
	uint64_t exit_info_2;

	// 088h
	exit_int_info exit_int_info;

	// 090h
	union {
		uint64_t value90;
		struct {
			uint64_t np_enable : 1;		// np_enable: enable nested paging.
			uint64_t secure_vm : 1;		// secure_vm: enable secure encrypted virtualization
			uint64_t encrypted_vm : 1;	// encrypted_vm: enable encrypted state for secure encrypted virtualization	
			uint64_t gme : 1;			// gme: guest mode execute trap
			uint64_t ssschecken : 1;	// ssschecken - enable supervisor shadow stack restrictions in nested page tables.support for this feature is indicated by cpuid fn8000_000a_edx[19](ssscheck)
			uint64_t vte : 1; 		// vte: virtual transparent encryption.
			uint64_t readonly_gpt : 1;	// readonly_gpt: enable read only guest page tables. 
			uint64_t invlpgb_tlbsync : 1;	// invlpgb_tlbsync: enable invlpgb/tlbsync. 0 - invlpgb and tlbsync will result in #ud. 1 - invlpgb and tlbsync can be executed in guest. presence of this bit is indicated by cpuid bit 8000_000a.edx[24] = 1
			uint64_t reserved8 : 56;	// reserved
		};
	};

	// 098h
	union {
		uint64_t value98;
		struct {
			uint64_t avic_apic_bar : 52;
			uint64_t reserved : 12;
		};
	};

	// 0a0h
	uint64_t gpa_of_ghcb;

	// 0a8h
	uint64_t event_injection;

	// 0b0h
	uint64_t n_cr3; // nested page table cr3 to use for nested paging


	// 0b8h
	union {
		uint64_t valueb8;
		struct {
			uint64_t lbr_virtualization_enable : 1;					// lbr_virt: 0—do nothing 1—enable lbr virtualization hardware acceleratio
			uint64_t virtualized_vmsave_vmload : 1;					// virt_vmsave_vmload: virtualized vmsave/vmload (enable)
			uint64_t virtualized_instruction_based_sampling : 1;	// virtualized_instruction_based_sampling: virtualized instruction-based sampling (enable)
			uint64_t reserved3 : 61;								// reserved
		};
	};

	// 0c0h
	union {
		uint64_t valuec0;
		struct {
			// clean bits 0:31 :
			uint64_t i : 1;		// i: intercepts: all the intercept vectors, tsc offset, pause filter count
			uint64_t iopm : 1;	// iomsrpm: iopm_base, msrpm_base
			uint64_t asid : 1;
			uint64_t tpr : 1; // v_tpr, v_irq, v_intr_prio, v_ign_tpr, v_intr_masking, v_intr_vector (offset 60h–67h)
			uint64_t np : 1; // nested paging: ncr3, g_pat
			uint64_t crx : 1; // cr0, cr3, cr4, efer
			uint64_t drx : 1; // dr6, dr7
			uint64_t dt : 1; // gdt/idt limit and base
			uint64_t seg : 1; // cs/ds/ss/es sel/base/limit/attr, cpl
			uint64_t cr2 : 1; // cr2
			uint64_t lbr : 1; // dbgctlmsr, br_from/to, lastint_from/to
			uint64_t avic : 1; // avic apic_bar; avic apic_backing_page, avic physical_table and avic logical_table pointers
			uint64_t cet : 1; //  s_cet, ssp, isst_addr
			uint64_t reserved13 : 19; // reserved for future compatibility
			// **************
			uint64_t reserved32 : 32;
		};
	};

	// 0c8h
	uint64_t nrip; // nrip—next sequential instruction pointer

	// 0d0h
	uint64_t guest_intruction_bytes[2]; // 0:7 number of bytes fetched. 8:127 intruction bytes

	// 0e0h
	union {
		uint64_t valuee0;
		struct {
			uint64_t avic_apic_backing_page_pointer : 52;
			uint64_t reserved52 : 12;
		};
	};

	// 0e8h - 0efh reserved
	uint8_t reserved0e8[7];

	// 0f0h
	union {
		uint64_t valuef0;
		struct {
			uint64_t reserved0 : 12;
			uint64_t avic_logical_table_pointer : 40;
			uint64_t reserved52 : 12;
		};
	};

	// 0f8h
	union {
		uint64_t valuef8;
		struct {
			uint64_t avic_physical_max_index : 8;
			uint64_t reserved8 : 4;
			uint64_t avic_physical_table_pointer : 40;
			uint64_t reserved52 : 12;
		};
	};

	// 100h - 10hh reserved
	uint8_t reserved100[7];

	// 108h
	union {
		uint64_t value108;
		struct {
			uint64_t reserved0 : 12;
			uint64_t vmsa_pointer : 40;
			uint64_t reserved52 : 12;
		};
	};

	// 110h
	uint64_t vmgexit_rax;

	// 118h
	uint8_t vmgexit_cpl;

	// 119h
	uint8_t reserved119[7];

	// 120h
	uint16_t bus_lock_threshhold_counter;

	// 122h - 3dfh 
	uint8_t reserved122[0x2bd];

	// 3e0h - 3ffh reserved for host usage
	uint8_t host_reserved3e0[0x1f];
};