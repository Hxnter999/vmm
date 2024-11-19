#include <vmexit/handlers.h>
#include <cpuid/cpuid_t.h>

// Not recommended to intercept cpuid cause its very commonly used for timing checks and AMD gives u the flexibility of choosing if u want to intercept it or not unlike intel. We choose not to
void cpuid_handler(vcpu_t& cpu) {
	cpuid::cpuid_t result{};
	__cpuidex(reinterpret_cast<int*>(&result.cpu_info), cpu.ctx.rax.low, cpu.ctx.rcx.low);
	
	cpu.ctx.rax.value = result.registers.eax;
	cpu.ctx.rbx.value = result.registers.ebx;
	cpu.ctx.rcx.value = result.registers.ecx;
	cpu.ctx.rdx.value = result.registers.edx;

	cpu.skip_instruction();
}