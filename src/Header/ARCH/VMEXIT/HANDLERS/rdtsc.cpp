#include "../handlers.h"

HANDLER_STATUS rdtsc_handler(vcpu_t& vcpu) 
{
	if (vcpu.guest_vmcb.save_state.cr4.tsd && vcpu.guest_vmcb.save_state.cpl && vcpu.guest_vmcb.save_state.cr0.pe) 
	{
		if (vcpu.guest_vmcb.save_state.cpl < 4) //spec weirdness
			return HANDLER_STATUS::INJECT_GP;
		return HANDLER_STATUS::INCREMENT_RIP;
	}

	register_t value{ __rdtsc() };
	vcpu.guest_stack_frame.rax = value.dword1;
	vcpu.guest_stack_frame.rdx = value.dword2;

	return HANDLER_STATUS::INCREMENT_RIP;
}