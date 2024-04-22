#include <vmexit/handlers.h>

static register_t rdmsr_handler(vcpu_t& vcpu, uint32_t msr);
static void wrmsr_handler(vcpu_t& vcpu, uint32_t msr, register_t result);

void msr_handler(vcpu_t& vcpu) {
	vcpu.guest_vmcb.state.rip = vcpu.guest_vmcb.control.nrip;

	bool read = vcpu.guest_vmcb.control.exit_info_1.msr.is_read();

	uint32_t msr = vcpu.guest_stack_frame.rcx.low;
	// MSR return value is split between 2 registers, we have to handle them both before passing it back into the guest.

	if (read) {
		register_t result = rdmsr_handler(vcpu, msr);
		vcpu.guest_stack_frame.rax.value = result.low;
		vcpu.guest_stack_frame.rdx.value = result.high;
	}

	else {
		wrmsr_handler(vcpu, msr, { 
			.low = vcpu.guest_stack_frame.rax.low, 
			.high = vcpu.guest_stack_frame.rdx.low 
		});
	}
}

static register_t rdmsr_handler(vcpu_t& vcpu, uint32_t msr) {
	switch (msr) {
	case MSR::EFER::MSR_EFER: 
	{
		MSR::EFER efer{};
		efer = vcpu.guest_vmcb.state.efer;
		efer.svme = 0;
		return { efer.bits };
	}
	case MSR::HSAVE_PA::MSR_VM_HSAVE_PA:
	{
		return { .value = 0 };
	}
	default:
	{
		return { __readmsr(msr) };
	}
	}
}

static void wrmsr_handler(vcpu_t& vcpu, uint32_t msr, register_t result) {
	switch (msr) {
	case MSR::EFER::MSR_EFER: 
	{
		MSR::EFER efer{};
		efer.bits = result.value;

		// write the msr anyway but flip svme, incase the guest is trying to enable/disable multiple things at once we dont want to discard everything.
		efer.svme = 1;
		vcpu.guest_vmcb.state.efer.bits = efer.bits;
		break;
	}
	case MSR::HSAVE_PA::MSR_VM_HSAVE_PA:
	{
		// TODO: cache changes requested by the guest and return them next time they read it
		print("HSAVE_PA write???\n");
		break;
	}
	default:
	{
		__writemsr(msr, result.value);
		break;
	}
	}
}