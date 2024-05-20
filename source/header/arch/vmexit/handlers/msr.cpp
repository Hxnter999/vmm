#include <vmexit/handlers.h>
#include <msrs/efer.h>
#include <msrs/msrs.h>
#include <msrs/hsave.h>

static register_t rdmsr_handler(vcpu_t& cpu, uint32_t msr);
static void wrmsr_handler(vcpu_t& cpu, uint32_t msr, register_t result);

void msr_handler(vcpu_t& cpu) {
	// MSR return value is split between 2 registers, we have to handle them both before passing it back into the guest.
	uint32_t msr = cpu.ctx.rcx.low;

	// TODO:
	// since the hypervisor reserved msrs implicitly cause a vmexit, if we actually want to return values we must check if they are within range so we dont cause a host exception
	// later inject an exception, im unsure currently if thats required so..
	if (msr >= MSR::msrpm_t::reserved_start && msr <= MSR::msrpm_t::reserved_end) {
		//cpu.inject_event(exception_vector::GP, 0);
		print("synthetic rdmsr: %x\n", msr);
		//return;
	}
	

	bool read = cpu.guest.control.exit_info_1.msr.is_read();
	if (read) {
		register_t result = rdmsr_handler(cpu, msr);
		cpu.ctx.rax.value = result.low;
		cpu.ctx.rdx.value = result.high;
	}

	else {
		wrmsr_handler(cpu, msr, { 
			.low = cpu.ctx.rax.low, 
			.high = cpu.ctx.rdx.low 
		});
	}

	cpu.skip_instruction();
}

static register_t rdmsr_handler(vcpu_t& cpu, uint32_t msr) {
	switch (msr) {
	case MSR::EFER::MSR_EFER: 
	{
		MSR::EFER efer{};
		efer = cpu.guest.state.efer;
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

static void wrmsr_handler(vcpu_t& cpu, uint32_t msr, register_t result) {
	switch (msr) {
	case MSR::EFER::MSR_EFER: 
	{
		MSR::EFER efer{};
		efer.bits = result.value;

		// write the msr anyway but flip svme, incase the guest is trying to modify multiple things at once we dont want to discard everything.
		efer.svme = 1;
		cpu.guest.state.efer.bits = efer.bits;
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