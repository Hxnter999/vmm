#include <vmexit/handlers.h>
#include <cpuid/cpuid_t.h>

void cpuid_handler(vcpu_t& vcpu) {
	vcpu.guest_vmcb.state.rip = vcpu.guest_vmcb.control.nrip;

	CPUID::cpuid_t result{};
	__cpuidex(reinterpret_cast<int*>(&result.cpu_info), vcpu.guest_context.rax.low, vcpu.guest_context.rcx.low);

	//print("CPUID: %X %X\n", vcpu.guest_context.rax.low, vcpu.guest_context.rcx.low);

	vcpu.guest_context.rax.value = result.registers.eax;
	vcpu.guest_context.rbx.value = result.registers.ebx;
	vcpu.guest_context.rcx.value = result.registers.ecx;
	vcpu.guest_context.rdx.value = result.registers.edx;
}