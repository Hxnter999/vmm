#pragma once
#include <vmcb/vmcb_control.h>
#include <vmcb/vmcb_state_save.h>
#include <stack_frame.h>
#include <msrs/msrs.h>
#include <pages/pages.h>

struct vmcb_t {
	// table b-1 (control area)
	vmcb_control control;
	static_assert(sizeof(vmcb_control) == 0x400, "vmcb control is not 0x400");

	// table b-2 (state save area)
	vmcb_state_save save_state;
	static_assert(sizeof(vmcb_state_save) == 0xC00, "vmcb state save is not 0xC00");
};
static_assert(sizeof(vmcb_t) == 0x1000, "vmcb size is not 0x1000");
