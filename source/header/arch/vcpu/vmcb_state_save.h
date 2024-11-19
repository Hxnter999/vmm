#pragma once
#include <commons.h>
#include <memory/segments.h>
#include <cr/control_registers.h>
#include <rflags.h>
#include <msrs/s_cet.h>
#include <msrs/efer.h>
#include <msrs/star.h>
#include <msrs/lstar.h>
#include <msrs/cstar.h>
#include <msrs/sfmask.h>

struct vmcb_state_save_t {
	// 000h
	segment_register_t es; // only lower 32 bits of base are implemented

	// 010h
	segment_register_t cs; // only lower 32 bits of base are implemented

	// 020h
	segment_register_t ss; // only lower 32 bits of base are implemented

	// 030h
	segment_register_t ds; // only lower 32 bits of base are implemented

	// 040h
	segment_register_t fs;

	// 050h
	segment_register_t gs;

	// 060h
	segment_register_t gdtr; //  only lower 16 bits of limit are implemented

	// 070h
	segment_register_t ldtr;

	// 080h
	segment_register_t idtr; // only lower 16 bits of limit are implemented

	// 090h
	segment_register_t tr;

	// 0a0h - 0cah
	uint8_t reserveda0[0x2b];

	// 0cbh
	uint8_t cpl; // if the guest is real-mode then the cpl is forced to 0; if the guest is virtual - mode then the cpl is forced to 3

	// 0cch
	uint32_t reservedcc;

	// 0d0h
	msr::efer efer;

	// 0d8h
	uint8_t reservedd8[0x70];

	// 148h
	cr4_t cr4;

	// 150h
	cr3_t cr3;

	// 158h
	cr0_t cr0;

	// 160h
	uint64_t dr7;

	// 168h
	uint64_t dr6;

	// 170h
	rflags_t rflags;

	// 178h
	uint64_t rip;

	// 180h
	uint8_t reserved180[0x58];

	// 1d8h
	uint64_t rsp;

	// 1e0h
	msr::s_cet s_cet;

	// 1e8h
	uint64_t ssp;

	// 1f0h
	uint64_t isst_addr;

	// 1f8h
	uint64_t rax;

	// 200h
	msr::star star;

	// 208h
	msr::lstar lstar;

	// 210h
	msr::cstar cstar;

	// 218h
	msr::sfmask sfmask;

	// 220h
	uint64_t kernel_gs_base;

	// 228h
	uint64_t sysenter_cs;

	// 230h
	uint64_t sysenter_esp;

	// 238h
	uint64_t sysenter_eip;

	// 240h
	cr2_t cr2;

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
	uint64_t last_excp_to;

	// 298h
	uint64_t dbg_extn_ctl;

	// 2a0h
	uint8_t reserved2a0[72];

	// 2e0h
	uint64_t spec_ctrl;

	// 2e8h
	uint8_t reserved2e8[904];

	// 670h
	uint8_t lbr_stack_from[128];
	uint8_t lbr_stack_to[128];

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
	uint64_t ibs_op_data;

	// 7a0h
	uint64_t ibs_op_data2;

	// 7a8h
	uint64_t ibs_op_data3;

	// 7b0h
	uint64_t ibs_dc_linear_address;

	// 7b8h
	uint64_t bp_ibstgt_rip;

	// 7c0h
	uint64_t ic_ibs_extd_ctl;

	// 7c8h
	uint8_t reserved7c8[1072];
};