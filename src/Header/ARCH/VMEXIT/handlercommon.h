#pragma once
#include <VMCB/vmcb_state_save.h>

bool is_arch_x64(const vmcb_state_save& save_state)
{
	return save_state.efer.lma && save_state.cs.attributes.longmode && !save_state.cs.attributes.default_bit;
}