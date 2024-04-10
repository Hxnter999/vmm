#include <arch/vmexit/handlers.h>
#include <ARCH/CPUID/standard-features/fn_processor.h>
#include <ARCH/CPUID/extended-features/fn_identifiers.h>

void cpuid_handler(vcpu_t& vcpu) {

	print("Buh!\n");

	CPUID::cpuid_t result{};
	int function_id = vcpu.guest_stack_frame.rax.low;
	int leaf_id = vcpu.guest_stack_frame.rcx.low;
	__cpuidex(reinterpret_cast<int*>(&result.cpu_info), function_id, leaf_id);

	print("function id: %d\n", function_id);
	//print("CPUID: %X %X\n", vcpu.guest_stack_frame.rax.low, vcpu.guest_stack_frame.rcx.low);
	switch (function_id) 
	{
	case CPUID::fn_processor::id: {
		CPUID::fn_processor* tm = reinterpret_cast<CPUID::fn_processor*>(&result);
		tm->feature_identifiers2.vme = 0;
		break;
	}
	case CPUID::fn_identifiers::id: {
		CPUID::fn_identifiers* tm = reinterpret_cast<CPUID::fn_identifiers*>(&result);
		tm->feature_identifiers_ext.vme = 0;
		break;
	}
	}


	vcpu.guest_stack_frame.rax.value = result.registers.eax;
	vcpu.guest_stack_frame.rbx.value = result.registers.ebx;
	vcpu.guest_stack_frame.rcx.value = result.registers.ecx;
	vcpu.guest_stack_frame.rdx.value = result.registers.edx;

	INCREMENT_RIP;
}