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
	uint8_t reserved60[6]; // reserved placeholders for selector and attribute of gdtr

	// 066h
	dtr gdtr; //  only lower 16 bits of limit are implemented

	// 070h
	SEGMENT::segment_register ldtr;

	// 080h
	uint8_t reserved80[6]; // reserved placeholders for selector and attribute of idtr

	// 086h
	dtr idtr; // only lower 16 bits of limit are implemented

	// 090h
	SEGMENT::segment_register tr;

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

	// 288h
	uint8_t last_excp_from[10];

	// 298h
	uint64_t dbg_extn_cfg;

	//manual says: "299h - 2DFh" which doesnt make sense as dbg_extn_cfg is a QWORD
	//Therfore I will go with the following to assure reserved299 ends at byte 0x2DF
	//0x2A0 - 0x2DF
	uint8_t reserved299[0x3F];

	// 2e0h
	uint64_t spec_ctrl;

	// 2E8h
	uint8_t reserved2e8[0x387];

	// 670h–76Fh 
	uint64_t lbr_stack_from[16];
	uint64_t lbr_stack_to[16];

	// 770h
	uint64_t lbr_select;

	// 778h
	uint64_t ibs_fetch_ctl;

	// 780h
	uint64_t ibs_fetch_linaddr;

	// 788h
	uint64_t ibs_op_ctl;

	// 790h
	uint64_t ibs_op_rip;

	// 798h
	uint64_t ibs_op_data1;

	// 7A0h
	uint64_t ibs_op_data2;

	// 7A8h
	uint64_t ibs_op_data3;

	// 7B0h
	uint64_t ibs_dc_linaddr;

	// 7B8h
	uint64_t bp_ibstgt_rip;

	// 7C0h
	uint64_t ic_ibs_extd_ctl;

	// 7C8h 
	uint8_t reservedbc7[0x430];
};