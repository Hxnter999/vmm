#include <arch/vmexit/handlers.h>

void cpuid_handler(vcpu_t& vcpu) {

	CPUID::cpuid_t result{};
	__cpuidex(reinterpret_cast<int*>(&result.cpu_info), vcpu.guest_stack_frame.rax.low, vcpu.guest_stack_frame.rcx.low);

	//print("CPUID: %X %X\n", vcpu.guest_stack_frame.rax.low, vcpu.guest_stack_frame.rcx.low);

	vcpu.guest_stack_frame.rax.value = result.registers.eax;
	vcpu.guest_stack_frame.rbx.value = result.registers.ebx;
	vcpu.guest_stack_frame.rcx.value = result.registers.ecx;
	vcpu.guest_stack_frame.rdx.value = result.registers.edx;
}