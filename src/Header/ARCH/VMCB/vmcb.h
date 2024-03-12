#pragma once
#include "vmcb_control.h"
#include "vmcb_state_save.h"
#include "../MSRs/msrs.h"

struct vmcb {
	// table b-1 (control area)
	vmcb_control control;
	static_assert(sizeof(vmcb_control) == 0x400, "vmcb control is not 0x400");
	// table b-2 (state save area)
	vmcb_state_save save_state;
	static_assert(sizeof(vmcb_state_save) == 0xC00, "vmcb state save is not 0xC00");
};
static_assert(sizeof(vmcb) == 0x1000, "vmcb size is not 0x1000");

struct stack_frame //stuff that isnt saved by vmcb
{
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rbx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;

	uint128_t xmm0;
	uint128_t xmm1;
	uint128_t xmm2;
	uint128_t xmm3;
	uint128_t xmm4;
	uint128_t xmm5;
};

struct vcpu {
	union {
		uint8_t host_stack[0x6000]; //0x6000 default size of KM stack
		struct {
			uint8_t stack_contents[0x6000 - (sizeof(uint64_t) * 4) - sizeof(stack_frame)];
			stack_frame guest_stack_frame;
			uint64_t guest_vmcb_pa; // host rsp
			uint64_t host_vmcb_pa;
			vcpu* self;
			struct shared* shared_data;
		};
	};
	vmcb host_vmcb;
	vmcb guest_vmcb;
	uint8_t host_state_area[0x1000]; //Do not modfiy (depends on chipset), just set phys (page alligned) to VM_HSAVE_PA
	bool is_virtualized;
};

//static_assert(sizeof(vcpu) == 0x3010 + 0x6000, "vcpu size is not 0x9010");

struct shared {
	vcpu* current_vcpu;
	vcpu* vcpus;
	uint32_t vcpu_count;
	MSR::msrpm* shared_msrpm;
};