#include "../commons.h"
#include "hypercall.h"

void hypercall_handler(vcpu* vcpu) {
	vcpu->guest_vmcb.save_state.rip = vcpu->guest_vmcb.control.nrip;

	switch (static_cast<hypercall_code>(vcpu->guest_stack_frame.rcx)) {
	case hypercall_code::UNLOAD:
	{
		vcpu->should_exit = true;
		// rcx -> nrip
		// rbx -> rsp
		vcpu->guest_stack_frame.rcx = vcpu->guest_vmcb.control.nrip;
		vcpu->guest_stack_frame.rbx = vcpu->guest_vmcb.save_state.rsp;
	
		__svm_vmload(vcpu->guest_vmcb_pa);
	
		_disable();
		__svm_stgi();
	
		MSR::EFER efer{}; efer.load(); efer.svme = 0; efer.store();
		__writeeflags(vcpu->guest_vmcb.save_state.rflags);
		break;
	}
	case hypercall_code::PING:
	{
		print("PONG\n");
		break;
	}
	default:
	{
		print("PONG\n");
		break;
	}
	}
}