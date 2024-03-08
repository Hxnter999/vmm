#pragma once
#include "vmcb_control.h"
#include "vmcb_state_save.h"
#include "../MSRs/msrs.h"

struct alignas(0x1000) vmcb {
	// table b-1 (control area)
	vmcb_control control;
	// table b-2 (state save area)
	vmcb_state_save save_state;
};

struct sharedvcpu 
{
	MSR::msrpm shared_msrpm{};
};

struct vcpu {
	vmcb host_vmcb;
	vmcb guest_vmcb;
	bool is_virtualized{};
	uint8_t host_state_area[0x1000]; //Do not modfiy (depends on chipset), just set phys (page alligned) to VM_HSAVE_PA
};