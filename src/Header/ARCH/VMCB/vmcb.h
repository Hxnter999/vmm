#pragma once
#include "vmcb_control.h"
#include "vmcb_state_save.h"
#include "../MSRs/msrs.h"

struct alignas(0x1000) vmcb {
	// table b-1 (control area)
	vmcb_control control;
	static_assert(sizeof(vmcb_control) == 0x400, "vmcb_control size is not 0x400");
	// table b-2 (state save area)
	vmcb_state_save save_state;
	static_assert(sizeof(vmcb_state_save) == 0xC00, "vmcb_state_save size is not 0xC00");

	static_assert(sizeof(vmcb_control) + sizeof(vmcb_state_save) == 0x1000, "vmcb size is not 0x1000");
};

struct sharedvcpu 
{
	MSR::msrpm* shared_msrpm{};
};

struct alignas(0x1000) vcpu {
	vmcb host_vmcb;
	vmcb guest_vmcb;
	//uint8_t host_state_area[0x1000]; //Do not modfiy (depends on chipset), just set phys (page alligned) to VM_HSAVE_PA
	bool is_virtualized;
};