#include <vmexit/handlers.h>
#include <msrs/efer.h>
#include <msrs/msrs.h>
#include <msrs/hsave.h>

static register_t rdmsr_handler(vcpu_t& vcpu, uint32_t msr);
static void wrmsr_handler(vcpu_t& vcpu, uint32_t msr, register_t result);

void msr_handler(vcpu_t& vcpu) {
	vcpu.guest_vmcb.state.rip = vcpu.guest_vmcb.control.nrip;

	// MSR return value is split between 2 registers, we have to handle them both before passing it back into the guest.
	uint32_t msr = vcpu.guest_context.rcx.low;

	// TODO:
	// since the hypervisor reserved msrs implicitly cause a vmexit, if we actually want to return values we must check if they are within range so we dont cause a host exception
	// later inject an exception, im unsure currently if thats required so..
	if (msr >= MSR::msrpm_t::reserved_start && msr <= MSR::msrpm_t::reserved_end) {
		vcpu.guest_context.rax.value = 0;
		vcpu.guest_context.rdx.value = 0;
		return;
	}

	bool read = vcpu.guest_vmcb.control.exit_info_1.msr.is_read();
	if (read) {
		print("rdmsr: %x\n", msr);
		register_t result = rdmsr_handler(vcpu, msr);
		vcpu.guest_context.rax.value = result.low;
		vcpu.guest_context.rdx.value = result.high;
	}

	else {
		wrmsr_handler(vcpu, msr, { 
			.low = vcpu.guest_context.rax.low, 
			.high = vcpu.guest_context.rdx.low 
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

		// write the msr anyway but flip svme, incase the guest is trying to modify multiple things at once we dont want to discard everything.
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