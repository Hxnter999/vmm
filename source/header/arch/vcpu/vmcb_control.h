#pragma once
#include <commons.h>
#include <vmexit/exit_int_info.h>
#include <vmexit/exitinfo1.h>
#include <vmexit/exitinfo2.h>
#include <msrs/msrs.h>
#include <vmexit/vmexit_code.h>
#include <vmexit/exit_int_info.h>

enum class tlb_control_id : uint64_t {
	do_nothing = 0,
	flush_entire_tlb = 1, // every entry, every asid; should only be used by legacy hypervisor
	flush_guest_tlb = 3, // flush this guest's tlb 
	flush_guest_non_global_tlb = 7 // flush this guest's non-global tlb
};

struct vmcb_control_t {
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
			uint32_t exception0_de : 1;
			uint32_t exception1_db : 1;
			uint32_t exception2_nmi : 1; // not supported. Manual: 15.12
			uint32_t exception3_bp : 1;
			uint32_t exception4_of : 1;
			uint32_t exception5_br : 1;
			uint32_t exception6_ud : 1;
			uint32_t exception7_nm : 1;
			uint32_t exception8_df : 1;
			uint32_t exception9_cso : 1;
			uint32_t exception10_ts : 1;
			uint32_t exception11_np : 1;
			uint32_t exception12_ss : 1;
			uint32_t exception13_gp : 1;
			uint32_t exception14_pf : 1;
			uint32_t exception15 : 1;
			uint32_t exception16_mf : 1;
			uint32_t exception17_ac : 1;
			uint32_t exception18_mc : 1;
			uint32_t exception19_xf : 1;
			uint32_t exception20 : 1;
			uint32_t exception21_cp : 1;
			uint32_t exception22 : 1;
			uint32_t exception23 : 1;
			uint32_t exception24 : 1;
			uint32_t exception25 : 1;
			uint32_t exception26 : 1;
			uint32_t exception27 : 1;
			uint32_t exception28_hv : 1;
			uint32_t exception29_vc : 1;
			uint32_t exception30_sx : 1;
			uint32_t exception31 : 1;
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
			uint32_t vmload : 1;
			uint32_t vmsave : 1;
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
			uint32_t efer_write_trap : 1; // TRAP intercepts occur after guest instructions finish executing
			uint32_t cr0_write_trap : 1;
			uint32_t cr1_write_trap : 1;
			uint32_t cr2_write_trap : 1;
			uint32_t cr3_write_trap : 1;
			uint32_t cr4_write_trap : 1;
			uint32_t cr5_write_trap : 1;
			uint32_t cr6_write_trap : 1;
			uint32_t cr7_write_trap : 1;
			uint32_t cr8_write_trap : 1;
			uint32_t cr9_write_trap : 1;
			uint32_t cr10_write_trap : 1;
			uint32_t cr11_write_trap : 1;
			uint32_t cr12_write_trap : 1;
			uint32_t cr13_write_trap : 1;
			uint32_t cr14_write_trap : 1;
			uint32_t cr15_write_trap : 1;
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

	//18h-3bh
	uint8_t reserved18[0x24];

	// 03ch 
	uint16_t pause_filter_threshold;

	// 03eh 
	uint16_t pause_filter_count;

	// 040h 
	uint64_t iopm_base_pa; //Should always be aligned

	// 048h 
	uint64_t msrpm_base_pa; //Should always be aligned 

	// 050h 
	uint64_t tsc_offset;

	// 058h 
	union {
		uint64_t value58;
		struct {
			uint64_t guest_asid : 32; // guest asid
			// tlb_control: tlb control bits
			tlb_control_id tlb_control : 8;
			uint64_t reserved40 : 24;
		};
	};

	// 060h 
	union {
		uint64_t value60;
		struct {
			uint64_t v_tpr : 8; // virtual tpr for guest cr8 operations
			uint64_t v_irq : 1; // if set, virtual interrupt is pending
			uint64_t v_gif : 1; // if set, virtual interrupts are unmasked 
			uint64_t v_nmi : 1; // if set, nmi is pending
			uint64_t v_nmi_mask : 1; // if set, v_nmi is masked
			uint64_t reserved12 : 3;
			uint64_t v_intr_prio : 4; // virtual interrupt priority
			uint64_t v_ign_tpr : 1; // if set, the current virtual interrupt is ignores v_tpr
			uint64_t reserved21 : 3;
			uint64_t v_intr_masking : 1; // virtualize masking of INTR interrupts, virtualizes the IF flag and cr8 access to v_tpr
			uint64_t v_gif_enable : 1; // if set, virtual gif is enabled for the guest
			uint64_t v_nmi_enable : 1; // nmi virtualization enabled
			uint64_t reserved27 : 3;
			uint64_t x2avic_enable : 1; // x2apic virtualization enabled
			uint64_t avic_enable : 1;
			uint64_t v_intr_vector : 8; // vector to use for this interrupt
			uint64_t reserved40 : 24;
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
	vmexit_code exit_code;

	// 078h
	exitinfo1_t exit_info_1;

	// 080h
	exitinfo2_t exit_info_2;

	// 088h
	exit_int_info_t exit_int_info;

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
	exit_int_info_t event_injection;

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
		struct { // write 1 to the specific bit when there are no changes to the specific register
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
		} clean_bits;
	};

	// 0c8h
	uint64_t nrip; // next sequential instruction pointer, only provided on INSTRUCTION intercepts

	// 0d0h
	// 0:7 number of bytes fetched. 8:127 intruction bytes, only used in when theres a #NPF
	uint8_t number_of_bytes_fetched;
	uint8_t guest_instructions[15];

	// 0e0h
	union {
		uint64_t valuee0;
		struct {
			uint64_t avic_apic_backing_page_pointer : 52;
			uint64_t reserved52 : 12;
		};
	};

	// 0e8h 
	uint64_t reserved0e8;

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

	// 100h
	uint64_t reserved100;

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
	uint64_t vmgexit_cpl;

	// 120h
	uint16_t bus_lock_threshold_counter;

	// 122h - 400h reserved (has other useless stuff we dont need + 32 bytes for host usage)
	uint8_t reserved122[0x2de];
};