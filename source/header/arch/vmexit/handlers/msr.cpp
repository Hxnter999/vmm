#include <vmexit/handlers.h>
#include <msrs/efer.h>
#include <msrs/msrs.h>
#include <msrs/hsave.h>

static register_t rdmsr_handler(vcpu_t& cpu, uint32_t msr);
static void wrmsr_handler(vcpu_t& cpu, uint32_t msr, register_t result);

/* 
* changes the guest makes will be reflected back but with limitations that we set (efer.svme bit for example), they will be cached and returned whenever attempted to read. 
* This is the case for the currently intercepted MSRs which are the only one that are somewhat required to be intercepted to keep the hypervisor hidden.
*/

/*
* Msrs to look out for, on amd would be:
*  - Efer
*  - Host vmcb msr which has the physical address of the host vmcb, referred to as hsave_pa
*  - Performance counters which track guest and host events independently
*  - Some other virtualization-related msrs which should not be used by a normal system like the doorbell msr, page flush msr etc.
*/

void msr_handler(vcpu_t& cpu) {
	uint32_t msr = cpu.ctx.rcx.low;

	if (msr >= msr::msrpm_t::reserved_start && msr < msr::msrpm_t::reserved_end) {
		cpu.inject_exception(exception_vector::GP, 0, true);
		return;
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
	case msr::efer::number: 
	{
		//print("[RDMSR] EFER\n");
		return { cpu.shadow.efer.value };
	}
	case msr::hsave_pa::number:
	{
		//print("[RDMSR] HSAVE_PA\n");
		return { cpu.shadow.hsave_pa.value };
	}
	default:
	{
		return { .value = __readmsr(msr) };
	}
	}
}

static void wrmsr_handler(vcpu_t& cpu, uint32_t msr, register_t result) {
	switch (msr) {
	case msr::efer::number: 
	{
		msr::efer new_efer{ .value = result.value };
		auto& old_efer = cpu.guest.state.efer;
		auto& shadow_efer = cpu.shadow.efer;
		//print("[WRMSR] EFER: %zX\n", new_efer.value);

		// TODO: handle reserved bits and system configuration bits and inject an exception respectively ...


		old_efer.value = new_efer.value;
		old_efer.svme = 1; // always enforce svme but still respect guest's value
		
		shadow_efer.value = new_efer.value; // cache the value and return it upon read, this ensures our changes are not directly visible but still vulnerable to introspection
		break;
	}
	case msr::hsave_pa::number:
	{
		msr::hsave_pa new_hsave {.value = result.value};
		auto& shadow_hsave = cpu.shadow.hsave_pa;
		//print("[WRMSR] HSAVE_PA: %zX\n", new_hsave.value);

		if (new_hsave.must_be_zero) { // address must be page aligned
			cpu.inject_exception(exception_vector::GP, 0, true);
			return;
		}

		// TODO: if outside the maximum supported physical address range, #GP

		shadow_hsave.value = new_hsave.value;
		break;
	}
	default:
	{
		__writemsr(msr, result.value);
		break;
	}
	}
}