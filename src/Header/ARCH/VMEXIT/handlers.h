#pragma once
#include "../VMCB/vmcb.h"

inline void msr_handler(vcpu* vcpu) {
	vcpu->guest_vmcb.save_state.rip = vcpu->guest_vmcb.control.nrip;

	uint64_t value = 0;
	bool read = vcpu->guest_vmcb.control.exit_info_1.msr.is_read();

	switch (vcpu->guest_stack_frame.rcx) {
	case MSR::EFER::MSR_EFER:
	{
		if (read) {
			MSR::EFER efer{}; efer.load();
			efer.svme = 0;
			value = efer.bits;
		}
		else 
			__writemsr(static_cast<uint32_t>(vcpu->guest_stack_frame.rcx), vcpu->guest_stack_frame.rdx);
		
		break;
	}
	case MSR::HSAVE_PA::MSR_VM_HSAVE_PA:
	{
		// value is initilized at 0, for both operations
		value = 0;
		break;
	}
	default:
	{
		if (read)
			value = __readmsr(static_cast<uint32_t>(vcpu->guest_stack_frame.rcx));
		else
			__writemsr(static_cast<uint32_t>(vcpu->guest_stack_frame.rcx), vcpu->guest_stack_frame.rdx);
		break;
	}
	}
	vcpu->guest_stack_frame.rax = value;
}