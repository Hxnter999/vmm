#pragma once
#include "vmcb_control.h"
#include "vmcb_state_save.h"
#include "../MSRs/msrs.h"
#include "../PAGES/pages.h"
#include "stack_frame_t.h"

struct vmcb_t {
	// table b-1 (control area)
	vmcb_control control;
	static_assert(sizeof(vmcb_control) == 0x400, "vmcb control is not 0x400");

	// table b-2 (state save area)
	vmcb_state_save save_state;
	static_assert(sizeof(vmcb_state_save) == 0xC00, "vmcb state save is not 0xC00");
};
static_assert(sizeof(vmcb_t) == 0x1000, "vmcb size is not 0x1000");

struct alignas(0x1000) vcpu_t {
	union {
		uint8_t host_stack[0x6000]; //0x6000 default size of KM stack
		struct {
			uint8_t stack_contents[0x6000 - (sizeof(uint64_t) * 4) - sizeof(stack_frame_t)];
			stack_frame_t guest_stack_frame; 
			uint64_t guest_vmcb_pa; // host rsp
			vcpu_t* self;
			uint64_t is_virtualized; // 16byte aligned
			uint64_t should_exit;
		};
	};
	vmcb_t host_vmcb;
	vmcb_t guest_vmcb;
	uint8_t host_state_area[0x1000]; //Do not modfiy (depends on chipset), just set phys (page alligned) to VM_HSAVE_PA
};
static_assert(sizeof(vcpu_t) == 0x9000, "vcpu size is not 0x9000");

struct shared {
	vcpu_t* current_vcpu;
	vcpu_t* vcpus;
	uint32_t vcpu_count;
	MSR::msrpm_t* shared_msrpm;
	uint64_t* npt;
};

inline shared global{};