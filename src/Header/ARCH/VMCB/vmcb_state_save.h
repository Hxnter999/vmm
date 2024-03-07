#pragma once
#include "datatypes.h"
#include "segment.h"
#include "drt.h"

struct vmcb_state_save {
	// 000h
	SEGMENT::segment_descriptor es;

	// 010h
	SEGMENT::segment_descriptor cs;

	// 020h
	SEGMENT::segment_descriptor ss;

	// 030h
	SEGMENT::segment_descriptor ds;

	// 040h
	SEGMENT::segment_descriptor fs;

	// 050h
	SEGMENT::segment_descriptor gs;

	// 060h
	dtr gdtr;

	// 070h
	SEGMENT::segment_descriptor ldtr;

	// 080h
	dtr idtr;

	// 090h
	SEGMENT::segment_descriptor tr;

	// 0a0h - 0cah
	uint8_t reserveda0[0x2a];

	// 0cbh
	uint8_t cpl; // if the guest is real-mode then the cpl is forced to 0; if the guest is virtual - mode then the cpl is forced to 3

	// 0cch
	uint32_t reservedcc;

	// 0d0h
	uint64_t efer;

	// 0d8h - 147h
	uint8_t reservedd8[0x70];

	// 148h
	uint64_t cr4;

	// 150h
	uint64_t cr3;

	// 158h
	uint64_t cr0;

	// 160h
	uint64_t dr7;

	// 168h
	uint64_t dr6;

	// 170h
	uint64_t rflags;

	// 178h
	uint64_t rip;

	// 180h - 1d7h
	uint8_t reserved180[0x58];

	// 1d8h
	uint64_t rsp;

	// 1e0h
	uint64_t s_cet;

	// 1e8h
	uint64_t ssp;

	// 1f0h
	uint64_t isst_addr;

	// 1f8h
	uint64_t rax;

	// 200h
	uint64_t star;

	// 208h
	uint64_t lstar;

	// 210h
	uint64_t cstar;

	// 218h
	uint64_t sfmask;

	// 220h
	uint64_t kernel_gs_base;

	// 228h
	uint64_t sysenter_cs;

	// 230h
	uint64_t sysenter_esp;

	// 238h
	uint64_t sysenter_eip;

	// 240h
	uint64_t cr2;

	// 248 - 267h 
	uint8_t reserved248[0x20];

	// 268h
	uint64_t g_pat;

	// 270h
	uint64_t dbgctl;

	// 278h
	uint64_t br_from;

	// 280h
	uint64_t br_to;

	// 288h to 7c8 might be wrong, possibly off by 8 due to shitty manuals
	uint64_t last_excp_from;

	// 290h
	uint64_t last_excp_to;

	// 299h
	uint64_t dbg_extn_cfg;

	// 2A0h
	uint8_t reserved298[0x48];

	// 2e8h
	uint64_t spec_ctrl;

	// 2f0h
	uint8_t reserved2e8[0x388];

	// 6780h
	uint64_t lbr_stack_from[16];

	// 6f8h
	uint64_t lbr_stack_to[16];

	// 778h
	uint64_t lbr_select;

	// 780h
	uint64_t ibs_fetch_ctl;

	// 788h
	uint64_t ibs_fetch_linaddr;

	// 790h
	uint64_t ibs_op_ctl;

	// 798h
	uint64_t ibs_op_rip;

	// 7A0h
	uint64_t ibs_op_data1;

	// 7a8h
	uint64_t ibs_op_data2;

	// 7B0h
	uint64_t ibs_op_data3;

	// 7b8h
	uint64_t ibs_dc_linaddr;

	// 7c0h
	uint64_t bp_ibstgt_rip;

	// 7c8h
	uint64_t ic_ibs_extd_ctl;

	// 7d0h
	uint8_t reservedbc7[0x430];
};