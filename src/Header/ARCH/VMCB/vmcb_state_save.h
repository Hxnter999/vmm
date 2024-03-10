#pragma once
#include "../../datatypes.h"
#include "../SEGMENT/segment.h"
#include "../dtr.h"

struct vmcb_state_save {
	// 000h
	SEGMENT::segment_register es; // only lower 32 bits of base are implemented

	// 010h
	SEGMENT::segment_register cs; // only lower 32 bits of base are implemented

	// 020h
	SEGMENT::segment_register ss;	// only lower 32 bits of base are implemented

	// 030h
	SEGMENT::segment_register ds; // only lower 32 bits of base are implemented

	// 040h
	SEGMENT::segment_register fs;

	// 050h
	SEGMENT::segment_register gs;

	// 060h
	SEGMENT::segment_register gdtr; //  only lower 16 bits of limit are implemented

	// 070h
	SEGMENT::segment_register ldtr;

	// 080h
	SEGMENT::segment_register idtr; // only lower 16 bits of limit are implemented

	// 090h
	SEGMENT::segment_register tr;

	// 0a0h - 0cah
	uint8_t reserveda0[0x2b];

	// 0cbh
	uint8_t cpl; // if the guest is real-mode then the cpl is forced to 0; if the guest is virtual - mode then the cpl is forced to 3

	// 0cch
	uint32_t reservedcc;

	// 0d0h
	uint64_t efer;

	// 0d8h
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

	// 180h
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

	// 248h 
	uint8_t reserved248[0x20];

	// 268h
	uint64_t g_pat;

	// 270h
	uint64_t dbgctl;

	// 278h
	uint64_t br_from;

	// 280h
	uint64_t br_to;

	// 288h
	uint64_t last_excp_from;

	// 290h
	uint64_t dbgextncfg;

	// 298h
	uint8_t reserved2a0[72];

	// 2e0h
	uint64_t spec_ctrl;

	// 2e8h
	uint8_t reserved2e8[904];

	// 670h - 770h
	uint8_t lbr_srack_from[128];
	uint8_t lbr_srack_to[128];

	// 770h
	uint64_t lbr_select;

	// 778h
	uint64_t ibs_fetch_ctl;

	// 780h
	uint64_t ibs_fetch_linear_address;

	// 788h
	uint64_t ibs_op_ctl;

	// 790h
	uint64_t ibs_op_rip;

	// 798h
	uint64_t ibs_op_data[3];

	// 7b0h
	uint64_t ibs_dc_linear_address;

	// 7b8h
	uint64_t bp_ibstgt_rip;

	// 7c0h
	uint64_t ic_ibs_extd_ctl;

	// 7c8h
	uint8_t reserved7c8[0x438];
};