#include <vmexit/handlers.h>
#include <cpuid/cpuid_t.h>

// Not recommended to intercept cpuid unless you are dynamically analyzing the guest
void cpuid_handler(vcpu_t& cpu) {
	cpu.skip_instruction();

	CPUID::cpuid_t result{};
	__cpuidex(reinterpret_cast<int*>(&result.cpu_info), cpu.ctx.rax.low, cpu.ctx.rcx.low);


	cpu.ctx.rax.value = result.registers.eax;
	cpu.ctx.rbx.value = result.registers.ebx;
	cpu.ctx.rcx.value = result.registers.ecx;
	cpu.ctx.rdx.value = result.registers.edx;
}