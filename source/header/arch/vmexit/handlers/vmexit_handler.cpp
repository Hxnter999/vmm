#include <vmexit/handlers.h>
#include <vmm.h>

bool vmexit_handler(vcpu_t& cpu) {
	__svm_vmload(cpu.host_vmcb_pa);

	// we copy rax and rsp into guest context so we can easily index gpr array and only modify guest ctx when register modificatios are required
	// this makes cleaner code if u modify from cpu.ctx rather than cpu.guest.state
	cpu.ctx.rax.value = cpu.guest.state.rax;
	cpu.ctx.rsp.value = cpu.guest.state.rsp;

	switch (cpu.guest.control.exit_code) {

	case vmexit_code::VMMCALL:
		print("VMMCALL\n");
		hypercall_handler(cpu);
		break;

	case vmexit_code::MSR:
		msr_handler(cpu);
		break;

	case vmexit_code::CPUID:
		cpuid_handler(cpu);
		break;

	case vmexit_code::INVALID:
		print("INVALID GUEST STATE, EXITING...\n");
		cpu.should_exit = true;
		break;

	case vmexit_code::NPF:
		print("[NPF] %zX\n", cpu.guest.control.exit_info_1.info);
		print("[NPF] %zX\n", cpu.guest.control.exit_info_2.nested_page_fault.faulting_gpa);
		npf_handler(cpu);
		break;

	case vmexit_code::HV: // event injection exception
		print("Failed to inject event\n");
		cpu.guest.control.event_injection.bits = 0; // reset to avoid infinite loop incase cpu doesnt clear it
		break;

	case vmexit_code::VMLOAD:
		svm_handler(cpu);
		break;

	case vmexit_code::VMSAVE:
		svm_handler(cpu);
		break;

	case vmexit_code::VMRUN:
		svm_handler(cpu);
		break;

	case vmexit_code::CLGI:
		svm_handler(cpu);
		break;

	case vmexit_code::STGI:
		svm_handler(cpu);
		break;

	case vmexit_code::SKINIT:
		svm_handler(cpu);
		break;

	case vmexit_code::CR4_WRITE:
		cr4_handler(cpu);
		break;

	default:
		print("UNHANDLED EXIT CODE: %-4X || INFO1: %p | INFO2: %p\n", cpu.guest.control.exit_code, cpu.guest.control.exit_info_1.info, cpu.guest.control.exit_info_2.info);
		break;
	}

	// copy over the changes into the vmcb so the cpu can load them on next vmrun
	cpu.guest.state.rax = cpu.ctx.rax.value;
	cpu.guest.state.rsp = cpu.ctx.rsp.value;

	if (cpu.should_exit) { // TODO: devirtualize by firing IPIs and handling them and remove this dogshit
		unload_single_vcpu(cpu); // devirtualize current vcpu and alert all others
		return false;
	};

	return true; // continue looping
};