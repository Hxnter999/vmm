#pragma once
#include "vmcb_control.h"
#include "vmcb_state_save.h"

struct alignas(0x1000) vmcb {
	// table b-1 (control area)
	vmcb_control control;
	// table b-2 (state save area)
	vmcb_state_save state_save;
};


