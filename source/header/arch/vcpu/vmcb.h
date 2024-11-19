#pragma once
#include <vcpu/vmcb_control.h>
#include <vcpu/vmcb_state_save.h>
#include <context.h>
#include <msrs/msrs.h>
#include <memory/page_tables.h>

struct vmcb_t {
	// table b-1 (control area)
	vmcb_control_t control;
	static_assert(sizeof(vmcb_control_t) == 0x400, "vmcb control area is not 0x400");

	// table b-2 (state save area)
	vmcb_state_save_t state;
	static_assert(sizeof(vmcb_state_save_t) == 0xC00, "vmcb state area is not 0xC00");
};
static_assert(sizeof(vmcb_t) == 0x1000, "vmcb size is not 0x1000");