#pragma once
#include "../VMCB/vmcb.h"
#include "../MSRs/hsave_pa.h"
#include "../MSRs/efer.h"

inline void msr_handler(vcpu_t* vcpu) {
	// MSR return value is split between 2 registers, we have to handle them both before passing it back into the guest.
	MSR::msr_t result;
	uint32_t msr = vcpu->guest_stack_frame.rcx.low;
	bool read = vcpu->guest_vmcb.control.exit_info_1.msr.is_read();
	
	result.eax = vcpu->guest_stack_frame.rax.low;
	result.edx = vcpu->guest_stack_frame.rdx.low;

	if (msr == MSR::EFER::MSR_EFER) {
		MSR::EFER efer{};
		if (read) {
			efer.load(); efer.svme = 0; result.value = efer.bits;
		}
		else {
			efer.bits = result.value;
			if (!efer.svme) { // we cant disable svme.
				vcpu->guest_vmcb.save_state.efer = result.value;
			}
		}
	}
	else if (msr == MSR::HSAVE_PA::MSR_VM_HSAVE_PA) {
		if (read) {
			result.value = 0;
		}
		else {
			print("HSAVE_PA write???\n");
		}
	}
	else {
		if (read) {
			result.value = __readmsr(msr);
		}
		else {
			__writemsr(msr, result.value);
		}
	}

	if (read) {
		vcpu->guest_stack_frame.rax.value = result.eax;
		vcpu->guest_stack_frame.rdx.value = result.edx;
	}
}