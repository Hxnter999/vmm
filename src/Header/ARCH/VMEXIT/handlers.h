#pragma once
#include "../VMCB/vmcb.h"

inline void msr_handler(vcpu_t* vcpu) {
	// MSR return value is split between 2 registers, we have to handle them both before passing it back into the guest.
	ULARGE_INTEGER result;
	uint32_t msr = vcpu->guest_stack_frame.rcx & 0xFFFFFFFF;
	bool read = vcpu->guest_vmcb.control.exit_info_1.msr.is_read();
	
	result.LowPart = vcpu->guest_stack_frame.rax & 0xFFFFFFFF;
	result.HighPart = vcpu->guest_stack_frame.rdx & 0xFFFFFFFF;

	if (msr == MSR::EFER::MSR_EFER) {
		MSR::EFER efer{};
		if (read) {
			efer.load(); efer.svme = 0; result.QuadPart = efer.bits;
		}
		else {
			efer.bits = result.QuadPart;
			if (!efer.svme) { // we cant disable svme.
				vcpu->guest_vmcb.save_state.efer = result.QuadPart;
			}
		}
	}
	else if (msr == MSR::HSAVE_PA::MSR_VM_HSAVE_PA) {
		if (read) {
			result.QuadPart = 0;
		}
		else {
			print("HSAVE_PA write???\n");
		}
	}
	else {
		if (read) {
			result.QuadPart = __readmsr(msr);
		}
		else {
			__writemsr(msr, result.QuadPart);
		}
	}

	if (read) {
		vcpu->guest_stack_frame.rax = result.LowPart;
	}	vcpu->guest_stack_frame.rdx = result.HighPart;
}