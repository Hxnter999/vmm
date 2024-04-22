#include "../handlers.h"

HANDLER_STATUS rdtsc_handler(vcpu_t& vcpu) 
{
	if (vcpu.guest_vmcb.save_state.cr4.tsd && vcpu.guest_vmcb.save_state.cpl && vcpu.guest_vmcb.save_state.cr0.pe) 
	{
		if (vcpu.guest_vmcb.save_state.cpl < 4) //spec weirdness
			return HANDLER_STATUS::INJECT_GP;
		return HANDLER_STATUS::INCREMENT_RIP;
	}

	vcpu.guest_stack_frame.rax = vcpu.timing.shadow_tsc.LowPart;
	vcpu.guest_stack_frame.rdx = vcpu.timing.shadow_tsc.HighPart;

	return HANDLER_STATUS::INCREMENT_RIP;
}