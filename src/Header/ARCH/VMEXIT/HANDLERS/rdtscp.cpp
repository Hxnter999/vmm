#include "../handlers.h"
#include <CPUID/extended-features/fn_identifiers.h>
#include <ARCH/MSRs/tsc_aux.h>

HANDLER_STATUS rdtscp_handler(vcpu_t& vcpu)
{
	static int8_t is_allowed = {
		-1
	};

	if (is_allowed == -1) {
		CPUID::fn_identifiers tk{};
		tk.load();
		is_allowed = tk.feature_identifiers_ext.rdtscp;
	}

	if (!is_allowed) return HANDLER_STATUS::INJECT_UD;

	if (vcpu.guest_vmcb.save_state.cr4.tsd && vcpu.guest_vmcb.save_state.cpl && vcpu.guest_vmcb.save_state.cr0.pe)
	{
		if (vcpu.guest_vmcb.save_state.cpl < 4) //spec weirdness
			return HANDLER_STATUS::INJECT_GP;
		return HANDLER_STATUS::INCREMENT_RIP;
	}

	vcpu.guest_stack_frame.rax = vcpu.timing.g_shadow.LowPart;
	vcpu.guest_stack_frame.rdx = vcpu.timing.g_shadow.HighPart;
	MSR::TSC_AUX aux{}; aux.load();
	vcpu.guest_stack_frame.rcx = aux.bits;
		
	return HANDLER_STATUS::INCREMENT_RIP;
}