#include <arch/vmexit/handlers.h>

void msr_handler(vcpu_t& vcpu) {
	// MSR return value is split between 2 registers, we have to handle them both before passing it back into the guest.

	register_t result{};
	uint32_t msr = vcpu.guest_stack_frame.rcx.low;
	bool read = vcpu.guest_vmcb.control.exit_info_1.msr.is_read();

	result.low = vcpu.guest_stack_frame.rax.low;
	result.high = vcpu.guest_stack_frame.rdx.low;

	if (msr == MSR::EFER::MSR_EFER) {
		MSR::EFER efer{};
		if (read) {
			efer.load(); efer.svme = 0; result.value = efer.bits;
		}
		else {
			efer.bits = result.value;
			//if (!efer.svme) { // we cant disable svme.
			//	vcpu.guest_vmcb.save_state.efer.bits = result.value;
			//}
			
			// allternatively, write the msr anyway but flip svme, incase the guest is trying to enable/disable multiple things at once we dont want to discard everything.
			efer.svme = 1;
			vcpu.guest_vmcb.save_state.efer.bits = efer.bits;
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
		vcpu.guest_stack_frame.rax.value = result.low;
		vcpu.guest_stack_frame.rdx.value = result.high;
	}
}